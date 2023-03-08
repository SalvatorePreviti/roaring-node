#ifndef ROARING_NODE_ADDON_DATA_
#define ROARING_NODE_ADDON_DATA_

#include "includes.h"
#include "addon-strings.h"

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

void AddonData_DeleteInstance(void * addonData) { delete (AddonData *)addonData; }

inline void AddonData_setMethod(
  v8::Local<v8::Object> recv, const char * name, v8::FunctionCallback callback, AddonData * addonData) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  v8::Local<v8::FunctionTemplate> t = v8::FunctionTemplate::New(isolate, callback, addonData->external.Get(isolate));
  v8::Local<v8::Function> fn = t->GetFunction(context).ToLocalChecked();
  v8::Local<v8::String> fn_name = v8::String::NewFromUtf8(isolate, name, v8::NewStringType::kInternalized).ToLocalChecked();
  fn->SetName(fn_name);
  recv->Set(context, fn_name, fn).Check();
}

#endif
