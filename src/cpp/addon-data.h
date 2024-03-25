#ifndef ROARING_NODE_ADDON_DATA_
#define ROARING_NODE_ADDON_DATA_

#include "includes.h"
#include "addon-strings.h"
#include <iostream>

template <typename T>
inline void ignoreMaybeResult(v8::Maybe<T>) {}

template <typename T>
inline void ignoreMaybeResult(v8::MaybeLocal<T>) {}

class AddonData;

void AddonData_WeakCallback(const v8::WeakCallbackInfo<AddonData> & info);

class AddonData final {
 public:
  v8::Isolate * isolate;
  v8::Persistent<v8::Object> persistent;

  v8::Eternal<v8::Object> Uint32Array;
  v8::Eternal<v8::Function> Uint32Array_from;
  v8::Eternal<v8::Function> Buffer_from;

  v8::Eternal<v8::FunctionTemplate> RoaringBitmap32_constructorTemplate;
  v8::Eternal<v8::Function> RoaringBitmap32_constructor;

  v8::Eternal<v8::FunctionTemplate> RoaringBitmap32BufferedIterator_constructorTemplate;
  v8::Eternal<v8::Function> RoaringBitmap32BufferedIterator_constructor;

  v8::Eternal<v8::External> external;

  AddonDataStrings strings;

  inline AddonData() : isolate(nullptr) {}

  ~AddonData() {
    std::cout << "AddonData::~AddonData" << std::endl;
    if (!this->persistent.IsEmpty()) {
      this->persistent.ClearWeak();
      this->persistent.Reset();
    }
  }

  static inline AddonData * get(const v8::FunctionCallbackInfo<v8::Value> & info) {
    v8::Local<v8::Value> data = info.Data();
    if (data.IsEmpty() || !data->IsExternal()) {
      return nullptr;
    }
    v8::Local<v8::External> external = v8::Local<v8::External>::Cast(data);
    return reinterpret_cast<AddonData *>(external->Value());
  }

  inline void initialize(v8::Isolate * isolate) {
    if (this->isolate) {
      return;
    }

    this->isolate = isolate;

    external.Set(isolate, v8::External::New(isolate, this));

    auto context = isolate->GetCurrentContext();

    auto global = context->Global();

    v8::Local<v8::Object> obj;
    if (v8::ObjectTemplate::New(isolate)->NewInstance(context).ToLocal(&obj)) {
      this->persistent.Reset(isolate, obj);
      this->persistent.SetWeak(this, AddonData_WeakCallback, v8::WeakCallbackType::kParameter);
    }

    this->strings.initialize(isolate);

    auto from = this->strings.from.Get(isolate);

    auto buffer = global->Get(context, this->strings.Buffer.Get(isolate)).ToLocalChecked().As<v8::Object>();
    this->Buffer_from.Set(isolate, buffer->Get(context, from).ToLocalChecked().As<v8::Function>());

    auto uint32Array =
      global->Get(context, this->strings.Uint32Array.Get(isolate)).ToLocalChecked()->ToObject(context).ToLocalChecked();

    auto uint32arrayFrom = v8::Local<v8::Function>::Cast(uint32Array->Get(context, from).ToLocalChecked());

    this->Uint32Array.Set(isolate, uint32Array);
    this->Uint32Array_from.Set(isolate, uint32arrayFrom);
  }

  void setMethod(v8::Local<v8::Object> recv, const char * name, v8::FunctionCallback callback) {
    v8::Isolate * isolate = this->isolate;
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::FunctionTemplate> t = v8::FunctionTemplate::New(isolate, callback, this->external.Get(isolate));
    v8::Local<v8::Function> fn = t->GetFunction(context).ToLocalChecked();
    v8::Local<v8::String> fn_name =
      v8::String::NewFromUtf8(isolate, name, v8::NewStringType::kInternalized).ToLocalChecked();
    fn->SetName(fn_name);
    ignoreMaybeResult(recv->Set(context, fn_name, fn));
  }
};

void AddonData_WeakCallback(const v8::WeakCallbackInfo<AddonData> & info) {
  AddonData * addonData = info.GetParameter();
  if (addonData) {
    delete addonData;
  }
}

#endif
