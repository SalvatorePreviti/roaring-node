#ifndef ROARING_NODE_ADDON_DATA_
#define ROARING_NODE_ADDON_DATA_

#include "includes.h"
#include "addon-strings.h"

void AddonData_DeleteInstance(void * addonData);

class AddonData final {
 public:
  AddonDataStrings strings;

  v8::Eternal<v8::Object> Uint32Array;
  v8::Eternal<v8::Function> Uint32Array_from;
  v8::Eternal<v8::Function> Buffer_from;

  std::atomic<uint64_t> RoaringBitmap32_instances;

  v8::Eternal<v8::FunctionTemplate> RoaringBitmap32_constructorTemplate;
  v8::Eternal<v8::Function> RoaringBitmap32_constructor;

  v8::Eternal<v8::FunctionTemplate> RoaringBitmap32BufferedIterator_constructorTemplate;
  v8::Eternal<v8::Function> RoaringBitmap32BufferedIterator_constructor;

  v8::Eternal<v8::External> external;

  inline AddonData() : RoaringBitmap32_instances(0) {}

  static inline AddonData * get(const v8::FunctionCallbackInfo<v8::Value> & info) {
    v8::Local<v8::Value> data = info.Data();
    if (data.IsEmpty()) {
      return nullptr;
    }
    v8::Local<v8::External> external = data.As<v8::External>();
    if (external.IsEmpty()) {
      return nullptr;
    }
    return reinterpret_cast<AddonData *>(external->Value());
  }

  inline void initialize(v8::Isolate * isolate) {
    v8::HandleScope scope(isolate);

    external.Set(isolate, v8::External::New(isolate, this));

    // node::AddEnvironmentCleanupHook(isolate, AddonData_DeleteInstance, this);

    this->strings.initialize(isolate);

    auto context = isolate->GetCurrentContext();

    auto global = context->Global();

    auto uint32Array = global->Get(context, NEW_LITERAL_V8_STRING(isolate, "Uint32Array", v8::NewStringType::kInternalized))
                         .ToLocalChecked()
                         ->ToObject(context)
                         .ToLocalChecked();

    this->Buffer_from.Set(
      isolate,
      global->Get(context, NEW_LITERAL_V8_STRING(isolate, "Buffer_from", v8::NewStringType::kInternalized))
        .ToLocalChecked()
        .As<v8::Function>());

    this->Uint32Array.Set(isolate, uint32Array);
    this->Uint32Array_from.Set(
      isolate,
      v8::Local<v8::Function>::Cast(
        uint32Array->Get(context, NEW_LITERAL_V8_STRING(isolate, "from", v8::NewStringType::kInternalized))
          .ToLocalChecked()));
  }
};

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

void AddonData_DeleteInstance(void * addonData) { delete (AddonData *)addonData; }

AddonData globalAddonData;

#endif
