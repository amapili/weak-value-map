#ifndef _JSMAP_H_
#define _JSMAP_H_
#include <napi.h>

class JSMap
{
private:
    Napi::ObjectReference map;

public:
    JSMap(const Napi::Env &env)
    {
        map = Napi::Persistent(env.Global().Get("Map").As<Napi::Function>().New({}));
    }
    JSMap &operator=(JSMap &&cpy)
    {
        map = std::move(cpy.map);
        return *this;
    }
    Napi::Value Get(Napi::Value val)
    {
        return map.Get("get").As<Napi::Function>().Call(map.Value(), {val});
    }
    Napi::Value Set(Napi::Value key, Napi::Value val)
    {
        return map.Get("set").As<Napi::Function>().Call(map.Value(), {key, val});
    }
    Napi::Value Delete(Napi::Value key)
    {
        return map.Get("delete").As<Napi::Function>().Call(map.Value(), {key});
    }
};

#endif
