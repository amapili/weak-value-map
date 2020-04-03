#ifndef _MAP_H_
#define _MAP_H_

#include <napi.h>
#include "jsmap.h"

class WeakValueMap : public Napi::ObjectWrap<WeakValueMap>
{
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    WeakValueMap(const Napi::CallbackInfo &info);

private:
    static Napi::FunctionReference constructor;
    static Napi::ObjectReference symbol;
    static Napi::Value toObject(Napi::Value val);
    static Napi::Value fromObject(Napi::Value val);
    Napi::Value Get(const Napi::CallbackInfo &info);
    Napi::Value Set(const Napi::CallbackInfo &info);
    Napi::Value Delete(const Napi::CallbackInfo &info);

    uint32_t size;
    JSMap map1;
    std::vector<Napi::Reference<Napi::Value>> refs1;
    JSMap map2;
    std::vector<Napi::Reference<Napi::Value>> refs2;
};

#endif
