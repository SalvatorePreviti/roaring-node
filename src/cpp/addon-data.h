#ifndef ROARING_NODE_ADDON_DATA_
#define ROARING_NODE_ADDON_DATA_

#include "includes.h"
#include "addon-strings.h"

class AddonData {
 public:
  AddonDataStrings strings;

  v8::Eternal<v8::Object> Uint32Array;
  v8::Eternal<v8::Function> Uint32Array_from;
  v8::Eternal<v8::Function> Buffer_from;

  v8::Eternal<v8::FunctionTemplate> RoaringBitmap32_constructorTemplate;
  v8::Eternal<v8::Function> RoaringBitmap32_constructor;

  v8::Eternal<v8::FunctionTemplate> RoaringBitmap32BufferedIterator_constructorTemplate;
  v8::Eternal<v8::Function> RoaringBitmap32BufferedIterator_constructor;

  void initialize(v8::Isolate * isolate) {
    v8::HandleScope scope(isolate);

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

AddonData globalAddonData;

#endif
