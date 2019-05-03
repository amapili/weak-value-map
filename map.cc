#include <node.h>
#include "object_wrap.h"
#include <unordered_map>
#include <tuple>
#include <string>

namespace {

struct Element;

using Map = std::unordered_map< std::string, Element >;

struct Element {
	Map &map;
	std::string const key;
	v8::Global<v8::Value> value;

	static void finalizationCb(v8::WeakCallbackInfo<Element> const &data) {
		auto element = data.GetParameter();
		element->map.erase(element->key);
	}

	void set(v8::Isolate *isolate, v8::Local<v8::Value> handle) {
		value.Reset(isolate, handle);
		value.SetWeak(this, finalizationCb, v8::WeakCallbackType::kParameter);
		value.MarkIndependent();
	}

	Element(Map &map, std::string key) : map{map}, key{key} {};
};

}  // anonymous namespace

class WeakValueMap : public ObjectWrap {
public:
	static void Init(v8::Local<v8::Object> exports) {
		auto isolate = exports->GetIsolate();

		//Create constructor funtion template
		auto tpl = v8::FunctionTemplate::New(isolate, New);
		tpl->SetClassName(v8::String::NewFromUtf8(isolate, "WeakValueMap"));
		tpl->InstanceTemplate()->SetInternalFieldCount(1);

		//Set class prototype methods
		NODE_SET_PROTOTYPE_METHOD(tpl, "set", Set);
		NODE_SET_PROTOTYPE_METHOD(tpl, "delete", Delete);
		NODE_SET_PROTOTYPE_METHOD(tpl, "get", Get);

		exports->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "WeakValueMap"), tpl->GetFunction(isolate->GetCurrentContext()).ToLocalChecked());
	}

private:
	Map map;

	static void New(v8::FunctionCallbackInfo<v8::Value> const &args) {
		auto isolate = args.GetIsolate();

		//Make sure this is a new-call or throw a type error
		if (!args.IsConstructCall()) {
			auto msg = v8::String::NewFromUtf8(isolate, "Constructor WeakValueMap requires 'new'");
			isolate->ThrowException(v8::Exception::TypeError(msg));
			return;
		}

		auto obj = new WeakValueMap();
		obj->Wrap(args.This());
		args.GetReturnValue().Set(args.This());
	}

	static void Set(v8::FunctionCallbackInfo<v8::Value> const &args) {
		//Delete from the map if the value to insert is undefined
		if (args[1]->IsUndefined())
			return WeakValueMap::Delete(args);

		auto isolate = args.GetIsolate();
		auto obj = ObjectWrap::Unwrap<WeakValueMap>(args.Holder());

		auto key = std::string(*v8::String::Utf8Value(isolate, args[0]->ToString(isolate)));
		auto i = obj->map.emplace( std::piecewise_construct,
				std::forward_as_tuple(key),
				std::forward_as_tuple(obj->map, key)).first;
		i->second.set(isolate, args[1]);

		args.GetReturnValue().Set(args.Holder());
	}

	static void Delete(v8::FunctionCallbackInfo<v8::Value> const &args) {
		auto isolate = args.GetIsolate();
		auto obj = ObjectWrap::Unwrap<WeakValueMap>(args.Holder());

		auto key = std::string(*v8::String::Utf8Value(isolate, args[0]->ToString(isolate)));
		obj->map.erase(key);

		args.GetReturnValue().Set(args.Holder());
	}

	static void Get(v8::FunctionCallbackInfo<v8::Value> const &args) {
		auto isolate = args.GetIsolate();
		auto obj = ObjectWrap::Unwrap<WeakValueMap>(args.Holder());

		auto key = std::string(*v8::String::Utf8Value(isolate, args[0]->ToString(isolate)));
		auto i = obj->map.find(key);
		if (i != obj->map.end())
			args.GetReturnValue().Set(i->second.value);
	}
};

NODE_MODULE(addon, WeakValueMap::Init)
