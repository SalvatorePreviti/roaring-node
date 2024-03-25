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
  static const constexpr uint64_t OBJECT_TOKEN = 0x2a5a4Fd152230110;

  v8::Isolate * isolate;
  v8::Persistent<v8::Symbol> addonDataSym;
  v8::Persistent<v8::Object> persistent;

  v8::Persistent<v8::Object> Uint32Array;
  v8::Persistent<v8::Function> Uint32Array_from;
  v8::Persistent<v8::Function> Buffer_from;

  v8::Persistent<v8::FunctionTemplate> RoaringBitmap32_constructorTemplate;
  v8::Persistent<v8::Function> RoaringBitmap32_constructor;

  v8::Persistent<v8::FunctionTemplate> RoaringBitmap32BufferedIterator_constructorTemplate;
  v8::Persistent<v8::Function> RoaringBitmap32BufferedIterator_constructor;

  AddonDataStrings strings;

  inline AddonData() : isolate(nullptr) { std::cout << ":AddonData " << this << std::endl; }

  ~AddonData() {
    std::cout << ":~AddonData " << this << std::endl;

    if (!this->persistent.IsEmpty()) {
      this->persistent.ClearWeak();
      this->persistent.Reset();
    }
  }

  static inline AddonData * get(const v8::FunctionCallbackInfo<v8::Value> & info) {
    v8::Local<v8::Value> data = info.Data();
    if (!data->IsObject()) {
      return nullptr;
    }
    v8::Local<v8::Object> obj;
    if (!data->ToObject(info.GetIsolate()->GetCurrentContext()).ToLocal(&obj)) {
      return nullptr;
    }
    if (obj->InternalFieldCount() != 2) {
      return nullptr;
    }
    if (obj->GetAlignedPointerFromInternalField(1) != reinterpret_cast<void *>(OBJECT_TOKEN)) {
      return nullptr;
    }
    return reinterpret_cast<AddonData *>(obj->GetAlignedPointerFromInternalField(0));
  }

  inline void initialize(v8::Isolate * isolate, v8::Local<v8::Object> exports) {
    std::cout << ":AddonData::initialize " << this << std::endl;
    if (this->isolate) {
      return;
    }

    this->isolate = isolate;

    auto context = isolate->GetCurrentContext();

    auto global = context->Global();

    this->addonDataSym.Reset(
      isolate,
      v8::Symbol::ForApi(
        isolate, v8::String::NewFromUtf8Literal(isolate, "RoaringAddonData", v8::NewStringType::kInternalized)));

    this->strings.initialize(isolate);

    v8::Local<v8::Object> obj;
    auto objT = v8::ObjectTemplate::New(isolate);
    objT->SetInternalFieldCount(2);

    if (objT->NewInstance(context).ToLocal(&obj)) {
      obj->SetAlignedPointerInInternalField(0, this);
      obj->SetAlignedPointerInInternalField(1, reinterpret_cast<void *>(OBJECT_TOKEN));

      this->persistent.Reset(isolate, obj);

      ignoreMaybeResult(exports->Set(context, this->addonDataSym.Get(isolate), obj));
    }

    auto from = this->strings.from.Get(isolate);

    auto buffer = global->Get(context, this->strings.Buffer.Get(isolate)).ToLocalChecked().As<v8::Object>();
    this->Buffer_from.Reset(isolate, buffer->Get(context, from).ToLocalChecked().As<v8::Function>());

    auto uint32Array =
      global->Get(context, this->strings.Uint32Array.Get(isolate)).ToLocalChecked()->ToObject(context).ToLocalChecked();

    auto uint32arrayFrom = v8::Local<v8::Function>::Cast(uint32Array->Get(context, from).ToLocalChecked());

    this->Uint32Array.Reset(isolate, uint32Array);
    this->Uint32Array_from.Reset(isolate, uint32arrayFrom);
  }

  inline void initializationCompleted() {
    if (!this->persistent.IsEmpty() && !this->persistent.IsWeak()) {
      this->persistent.SetWeak(this, AddonData_WeakCallback, v8::WeakCallbackType::kParameter);
    }
  }

  void setMethod(v8::Local<v8::Object> recv, const char * name, v8::FunctionCallback callback) {
    v8::Isolate * isolate = this->isolate;
    auto addonDataObj = this->persistent.Get(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::FunctionTemplate> t = v8::FunctionTemplate::New(isolate, callback, addonDataObj);
    v8::Local<v8::Function> fn = t->GetFunction(context).ToLocalChecked();
    v8::Local<v8::String> fn_name =
      v8::String::NewFromUtf8(isolate, name, v8::NewStringType::kInternalized).ToLocalChecked();
    fn->SetName(fn_name);
    ignoreMaybeResult(recv->Set(context, fn_name, fn));
    ignoreMaybeResult(fn->Set(context, this->addonDataSym.Get(isolate), addonDataObj));
  }
};

void AddonData_WeakCallback(const v8::WeakCallbackInfo<AddonData> & info) {
  std::cout << ":AddonData_WeakCallback " << info.GetParameter() << std::endl;
  AddonData * addonData = info.GetParameter();
  if (addonData) {
    addonData->persistent.ClearWeak();
    addonData->persistent.Reset();
    delete addonData;
  }
}

#endif
