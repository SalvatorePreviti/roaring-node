#ifndef ROARING_NODE_ADDON_DATA_
#define ROARING_NODE_ADDON_DATA_

#include "includes.h"
#include "addon-strings.h"
#include "object-wrap.h"
#include <iostream>

template <typename T>
inline void ignoreMaybeResult(v8::Maybe<T>) {}

template <typename T>
inline void ignoreMaybeResult(v8::MaybeLocal<T>) {}

class AddonData;

void AddonData_WeakCallback(const v8::WeakCallbackInfo<AddonData> & info);

void AddonData_Cleanup(void * param);

void AddonData_EmptyCallback(const v8::FunctionCallbackInfo<v8::Value> & info) {
  info.GetIsolate()->ThrowException(v8::Exception::Error(v8::String::Empty(info.GetIsolate())));
  info.GetReturnValue().Set(v8::Undefined(info.GetIsolate()));
}

class AddonData final {
 public:
  static const constexpr uint64_t OBJECT_TOKEN = 0x2a5a4Fd152230110;

  v8::Isolate * isolate;
  v8::Persistent<v8::Object> persistent;

  v8::Persistent<v8::Object> Uint32Array;
  v8::Persistent<v8::Function> Uint32Array_from;
  v8::Persistent<v8::Function> Buffer_from;

  v8::Persistent<v8::FunctionTemplate> RoaringBitmap32_constructorTemplate;
  v8::Persistent<v8::Function> RoaringBitmap32_constructor;

  v8::Persistent<v8::FunctionTemplate> RoaringBitmap32BufferedIterator_constructorTemplate;
  v8::Persistent<v8::Function> RoaringBitmap32BufferedIterator_constructor;

  AddonDataStrings strings;

  AddonData() : isolate(nullptr) { std::cout << ":AddonData " << this << std::endl; }

  ~AddonData() {
    std::cout << ":~AddonData " << this << std::endl;

    if (!this->persistent.IsEmpty()) {
      this->persistent.ClearWeak();
      this->persistent.Reset();
    }
    if (this->isolate) {
      this->isolate = nullptr;
      isolate->AdjustAmountOfExternalAllocatedMemory(-sizeof(this));
    }
  }

  static inline AddonData * get(const v8::FunctionCallbackInfo<v8::Value> & info) {
    return ObjectWrap::TryUnwrap<AddonData>(info.Data(), info.GetIsolate());
  }

  void initialize(v8::Isolate * isolate, v8::Local<v8::Object> exports) {
    std::cout << ":AddonData::initialize " << this << std::endl;
    if (this->isolate) {
      return;
    }

    this->isolate = isolate;

    auto context = isolate->GetCurrentContext();

    auto global = context->Global();

    this->strings.initialize(isolate);

    auto objT = v8::ObjectTemplate::New(isolate);
    objT->SetInternalFieldCount(2);

    v8::Local<v8::Object> obj = objT->NewInstance(context).ToLocalChecked();

    int indices[2] = {0, 1};
    void * values[2] = {this, (void *)(AddonData::OBJECT_TOKEN)};
    obj->SetAlignedPointerInInternalFields(2, indices, values);

    this->persistent.Reset(isolate, obj);
    isolate->AdjustAmountOfExternalAllocatedMemory(sizeof(this));

    auto from = this->strings.from.Get(isolate);

    auto buffer = global->Get(context, this->strings.Buffer.Get(isolate)).ToLocalChecked().As<v8::Object>();
    this->Buffer_from.Reset(isolate, buffer->Get(context, from).ToLocalChecked().As<v8::Function>());

    auto uint32Array =
      global->Get(context, this->strings.Uint32Array.Get(isolate)).ToLocalChecked()->ToObject(context).ToLocalChecked();

    auto uint32arrayFrom = v8::Local<v8::Function>::Cast(uint32Array->Get(context, from).ToLocalChecked());

    this->Uint32Array.Reset(isolate, uint32Array);
    this->Uint32Array_from.Reset(isolate, uint32arrayFrom);
  }

  void initializationCompleted() {
    std::cout << ":AddonData::initializationCompleted " << this << std::endl;
    node::AddEnvironmentCleanupHook(this->isolate, AddonData_Cleanup, this);
    this->persistent.SetWeak(this, AddonData_WeakCallback, v8::WeakCallbackType::kParameter);
  }

  void Cleanup() {
    v8::HandleScope scope(isolate);
    this->RoaringBitmap32_constructorTemplate.Reset();
    this->RoaringBitmap32_constructor.Reset();
    this->RoaringBitmap32BufferedIterator_constructorTemplate.Reset();
    this->RoaringBitmap32BufferedIterator_constructor.Reset();
  }

  void setStaticMethod(v8::Local<v8::Object> recv, const char * name, v8::FunctionCallback callback) {
    v8::Isolate * isolate = this->isolate;
    v8::HandleScope scope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::FunctionTemplate> t = v8::FunctionTemplate::New(isolate, callback, this->persistent.Get(isolate));
    v8::Local<v8::String> fn_name =
      v8::String::NewFromUtf8(isolate, name, v8::NewStringType::kInternalized).ToLocalChecked();
    t->SetClassName(fn_name);
    v8::Local<v8::Function> fn = t->GetFunction(context).ToLocalChecked();
    fn->SetName(fn_name);
    ignoreMaybeResult(recv->Set(context, fn_name, fn));
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

void AddonData_Cleanup(void * param) {
  std::cout << ":AddonData_Cleanup " << param << std::endl;
  AddonData * addonData = static_cast<AddonData *>(param);
  if (addonData) {
    addonData->Cleanup();
  }
}

#endif
