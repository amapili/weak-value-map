#include "map.h"
#include "jsmap.h"

#define DEFAULT_SIZE 1024

Napi::Object WeakValueMap::Init(Napi::Env env, Napi::Object exports)
{
    auto cons = DefineClass(env, "WeakValueMap", {InstanceMethod("get", &WeakValueMap::Get), InstanceMethod("set", &WeakValueMap::Set), InstanceMethod("delete", &WeakValueMap::Delete)});
    constructor = Napi::Persistent(cons);
    constructor.SuppressDestruct();

    auto obj = Napi::Object::New(env);
    obj.Set("symbol", Napi::Symbol::New(env));
    symbol = Napi::Persistent(obj);
    symbol.SuppressDestruct();

    exports.Set("WeakValueMap", cons);
    return exports;
}

WeakValueMap::WeakValueMap(const Napi::CallbackInfo &info) : Napi::ObjectWrap<WeakValueMap>(info), size(DEFAULT_SIZE), map1(info.Env()), map2(info.Env())
{
    if (info.Length())
    {
        auto args = info[0].As<Napi::Object>();
        if (args.Has("size"))
            size = args.Get("size").As<Napi::Number>();
    }
}

Napi::FunctionReference WeakValueMap::constructor;
Napi::ObjectReference WeakValueMap::symbol;

Napi::Value WeakValueMap::toObject(Napi::Value val)
{
    if (val.IsObject() || val.IsFunction())
        return val;
    auto obj = Napi::Object::New(symbol.Env());
    obj.Set(symbol.Get("symbol"), val);
    return obj;
}

Napi::Value WeakValueMap::fromObject(Napi::Value val)
{
    if (val.IsObject())
    {
        auto obj = val.As<Napi::Object>();
        auto s = symbol.Get("symbol");
        if (obj.Has(s))
            return obj.Get(s);
    }
    return val;
}

Napi::Value WeakValueMap::Get(const Napi::CallbackInfo &info)
{
    auto key = info[0];
    auto ind = map1.Get(key);
    if (ind.IsNumber())
    {
        return fromObject(refs1[(uint32_t)ind.As<Napi::Number>()].Value());
    }
    else
    {
        ind = map2.Get(key);
        if (ind.IsNumber())
        {
            auto ref = std::move(refs2[(uint32_t)ind.As<Napi::Number>()]);
            auto out = ref.Value();
            if (!ref.IsEmpty())
            {
                refs1.push_back(std::move(ref));
                map1.Set(key, Napi::Number::New(info.Env(), refs1.size() - 1));
            }
            return out;
        }
    }
    return info.Env().Undefined();
}

Napi::Value WeakValueMap::Set(const Napi::CallbackInfo &info)
{
    if (refs1.size() >= size)
    {
        refs2 = std::move(refs1);
        map2 = std::move(map1);
        map1 = JSMap(info.Env());
        refs1 = std::vector<Napi::Reference<Napi::Value>>();
    }
    auto key = info[0];
    auto val = info[1];
    refs1.push_back(Napi::Reference<Napi::Value>::New(toObject(val)));
    map1.Set(key, Napi::Number::New(info.Env(), refs1.size() - 1));
    return info.This();
}

Napi::Value WeakValueMap::Delete(const Napi::CallbackInfo &info)
{
    auto key = info[0];
    return map1.Delete(key);
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    WeakValueMap::Init(env, exports);
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
