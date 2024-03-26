#ifndef ROARING_NODE_OBJECT_WRAP_
#define ROARING_NODE_OBJECT_WRAP_

#include "includes.h"

namespace ObjectWrap {
  template <class T>
  static T * TryUnwrap(const v8::Local<v8::Value> & value, v8::Isolate * isolate) {
    v8::Local<v8::Object> obj;
    if (!value->IsObject()) {
      return nullptr;
    }
    if (!value->ToObject(isolate->GetCurrentContext()).ToLocal(&obj)) {
      return nullptr;
    }
    if (obj->InternalFieldCount() != 2) {
      return nullptr;
    }
    if ((uintptr_t)obj->GetAlignedPointerFromInternalField(1) != T::OBJECT_TOKEN) {
      return nullptr;
    }
    return reinterpret_cast<T *>(obj->GetAlignedPointerFromInternalField(0));
  }

  template <class T>
  static T * TryUnwrap(const v8::FunctionCallbackInfo<v8::Value> & info, int argumentIndex) {
    return info.Length() <= argumentIndex ? nullptr : ObjectWrap::TryUnwrap<T>(info[argumentIndex], info.GetIsolate());
  }

};  // namespace ObjectWrap

#endif  // ROARING_NODE_OBJECT_WRAP_
