#ifndef ROARING_NODE_OBJECT_WRAP_
#define ROARING_NODE_OBJECT_WRAP_

#include "addon-data.h"

class ObjectWrap {
 public:
  const uint64_t objectToken;
  AddonData * const addonData;

  template <class T>
  static T * TryUnwrap(const v8::Local<v8::Value> & value, v8::Isolate * isolate) {
    v8::Local<v8::Object> obj;
    if (isolate && value->ToObject(isolate->GetCurrentContext()).ToLocal(&obj)) {
      if (obj->InternalFieldCount() > 0) {
        T * result = (T *)(obj->GetAlignedPointerFromInternalField(0));
        if (result && result->objectToken == T::OBJECT_TOKEN) {
          return result;
        }
      }
    }
    return nullptr;
  }

  template <class T>
  static T * TryUnwrap(
    const v8::Local<v8::Value> & value, const v8::Local<v8::FunctionTemplate> & ctorTemplate, v8::Isolate * isolate) {
    return !ctorTemplate.IsEmpty() && ctorTemplate->HasInstance(value) ? ObjectWrap::TryUnwrap<T>(value, isolate) : nullptr;
  }

  template <class T>
  static T * TryUnwrap(
    const v8::Local<v8::Value> & value,
    const v8::Persistent<v8::FunctionTemplate, v8::CopyablePersistentTraits<v8::FunctionTemplate>> & ctorTemplate,
    v8::Isolate * isolate) {
    return ObjectWrap::TryUnwrap<T>(value, ctorTemplate.Get(isolate), isolate);
  }

  template <class T>
  static T * TryUnwrap(
    const v8::Local<v8::Value> & value, const v8::Eternal<v8::FunctionTemplate> & ctorTemplate, v8::Isolate * isolate) {
    return ObjectWrap::TryUnwrap<T>(value, ctorTemplate.Get(isolate), isolate);
  }

  template <class T>
  static T * TryUnwrap(
    const v8::FunctionCallbackInfo<v8::Value> & info,
    int argumentIndex,
    const v8::Local<v8::FunctionTemplate> & ctorTemplate) {
    return info.Length() <= argumentIndex ? nullptr
                                          : ObjectWrap::TryUnwrap<T>(info[argumentIndex], ctorTemplate, info.GetIsolate());
  }

  template <class T>
  static T * TryUnwrap(
    const v8::FunctionCallbackInfo<v8::Value> & info,
    int argumentIndex,
    const v8::Persistent<v8::FunctionTemplate, v8::CopyablePersistentTraits<v8::FunctionTemplate>> & ctorTemplate) {
    return info.Length() <= argumentIndex ? nullptr
                                          : ObjectWrap::TryUnwrap<T>(info[argumentIndex], ctorTemplate, info.GetIsolate());
  }

  template <class T>
  static T * TryUnwrap(
    const v8::FunctionCallbackInfo<v8::Value> & info,
    int argumentIndex,
    const v8::Eternal<v8::FunctionTemplate> & ctorTemplate) {
    return info.Length() <= argumentIndex ? nullptr
                                          : ObjectWrap::TryUnwrap<T>(info[argumentIndex], ctorTemplate, info.GetIsolate());
  }

 protected:
  explicit ObjectWrap(AddonData * addonData, uint64_t objectToken) : objectToken(objectToken), addonData(addonData) {}
};

#endif  // ROARING_NODE_OBJECT_WRAP_
