#include <node.h>
#include "object_wrap.h"
#include "utility.h"
#include <unordered_map>
#include <tuple>
#include <string>

using Args = v8::FunctionCallbackInfo<v8::Value> const;
using PropArgs = v8::PropertyCallbackInfo<v8::Value> const;


class WeakValueMap : public ObjectWrap {
public:
	//-------- public types ---------------------------------------------------------------

	using Key = std::string;

private:
	//-------- private types --------------------------------------------------------------

	struct Element;

	using Map = std::unordered_map< Key, Element >;

	struct Element {
		Map &map;
		Key const key;
		v8::Global<v8::Value> value;

		static void gc_callback( v8::WeakCallbackInfo<Element> const &info ) {
			auto element = info.GetParameter();
			element->map.erase( element->key );
		}

		void set( v8::Isolate *isolate, v8::Local<v8::Value> handle ) {
			value.Reset( isolate, handle );
			value.SetWeak( this, gc_callback, v8::WeakCallbackType::kParameter );
			value.MarkIndependent();
		}

		Element( Map &map, Key const &key ) : map{map}, key{key} {};
	};

	//-------- private members ------------------------------------------------------------

	Map map;

public:
	//-------- constructor ----------------------------------------------------------------

	static void constructor( Args &args ) {
		auto isolate = args.GetIsolate();

		//Make sure this is a new-call or throw a type error
		if (!args.IsConstructCall()) {
			auto msg = intern_string( isolate, "Class constructor "
					"WeakValueMap cannot be invoked without 'new'" );
			isolate->ThrowException(v8::Exception::TypeError(msg));
			return;
		}

		auto obj = new WeakValueMap();
		obj->Wrap(args.This());
		args.GetReturnValue().Set(args.This());
	}

	//-------- callback wrappers ----------------------------------------------------------

	template< void (WeakValueMap::*getter)( PropArgs & ) >
	static void wrap( v8::Local<v8::String> prop, PropArgs &args ) {
		auto obj = ObjectWrap::Unwrap<WeakValueMap>( args.Holder() );
		(obj->*getter)( args );
	}

	template< void (WeakValueMap::*method)( Args &, Key const & ) >
	static void wrap( Args &args ) {
		auto obj = ObjectWrap::Unwrap<WeakValueMap>( args.Holder() );
		auto &&key = v8::String::Utf8Value{ args.GetIsolate(), args[0]->ToString() };
		if( *key )
			(obj->*method)( args, *key );
	}

	//-------- properties -----------------------------------------------------------------

	void size_getter( PropArgs &args ) {
		args.GetReturnValue().Set( (uint32_t) map.size() );
	}

	//-------- methods --------------------------------------------------------------------

	void get_method( Args &args, Key const &key ) {
		auto i = map.find( key );
		if( i != map.end() )
			args.GetReturnValue().Set( i->second.value );
	}

	void set_method( Args &args, Key const &key ) {
		//Delete from the map if the value to insert is undefined
		if( args[1]->IsUndefined() )
			return delete_method( args, key );

		auto i = map.emplace( std::piecewise_construct,
				std::forward_as_tuple( key ),
				std::forward_as_tuple( map, key ) ).first;
		i->second.set( args.GetIsolate(), args[1] );

		args.GetReturnValue().Set( args.This() );
	}

	void delete_method( Args &args, Key const &key ) {
		map.erase( key );

		args.GetReturnValue().Set( args.This() );
	}
};


//-------- module initialization --------------------------------------------------------------

void initialize( v8::Local<v8::Object> exports, v8::Local<v8::Value> module,
		v8::Local<v8::Context> context )
{
	ObjectBuilder exp { context, exports };

	exp.add_class( "WeakValueMap", WeakValueMap::constructor, []( ClassBuilder cls ) {
		cls.set_internal_field_count( 1 );
		cls.add_property( "size", WeakValueMap::wrap<&WeakValueMap::size_getter> );
		cls.add_method( "get",    WeakValueMap::wrap<&WeakValueMap::get_method> );
		cls.add_method( "set",    WeakValueMap::wrap<&WeakValueMap::set_method> );
		cls.add_method( "delete", WeakValueMap::wrap<&WeakValueMap::delete_method> );
	});
}

NODE_MODULE_CONTEXT_AWARE( WeakValueMap, initialize )
