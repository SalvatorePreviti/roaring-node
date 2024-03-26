#ifndef ROARING_NODE_ADDON_DATA_
#define ROARING_NODE_ADDON_DATA_

#include "includes.h"
#include "addon-strings.h"
#include "object-wrap.h"
#include <unordered_set>
#include <mutex>

template <typename T>
inline void ignoreMaybeResult(v8::Maybe<T>) {}

template <typename T>
inline void ignoreMaybeResult(v8::MaybeLocal<T>) {}

class AddonData;

void AddonData_WeakCallback(const v8::WeakCallbackInfo<AddonData> & info);

void AddonData_Cleanup(void * param);

class AddonData;

class AddonData final {
 public:
  v8::Isolate * isolate;

  v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> Uint32Array;
  v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>> Uint32Array_from;
  v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>> Buffer_from;

  v8::Persistent<v8::FunctionTemplate, v8::CopyablePersistentTraits<v8::FunctionTemplate>>
    RoaringBitmap32_constructorTemplate;
  v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>> RoaringBitmap32_constructor;

  v8::Persistent<v8::FunctionTemplate, v8::CopyablePersistentTraits<v8::FunctionTemplate>>
    RoaringBitmap32BufferedIterator_constructorTemplate;
  v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>> RoaringBitmap32BufferedIterator_constructor;

  AddonDataStrings strings;

  explicit AddonData(v8::Isolate * isolate) : isolate(isolate) {
    v8::HandleScope scope(isolate);

    AddonData::AddonData_Init(this);

    auto context = isolate->GetCurrentContext();

    auto global = context->Global();

    this->strings.initialize(isolate);

    auto objT = v8::ObjectTemplate::New(isolate);
    objT->SetInternalFieldCount(2);

    isolate->AdjustAmountOfExternalAllocatedMemory(sizeof(this));

    auto from = v8::String::NewFromUtf8Literal(isolate, "from", v8::NewStringType::kInternalized);

    auto buffer = global->Get(context, v8::String::NewFromUtf8Literal(isolate, "Buffer", v8::NewStringType::kInternalized))
                    .ToLocalChecked()
                    .As<v8::Object>();

    this->Buffer_from.Reset(isolate, buffer->Get(context, from).ToLocalChecked().As<v8::Function>());

    auto uint32Array =
      global->Get(context, v8::String::NewFromUtf8Literal(isolate, "Uint32Array", v8::NewStringType::kInternalized))
        .ToLocalChecked()
        ->ToObject(context)
        .ToLocalChecked();

    auto uint32arrayFrom = v8::Local<v8::Function>::Cast(uint32Array->Get(context, from).ToLocalChecked());

    this->Uint32Array.Reset(isolate, uint32Array);
    this->Uint32Array_from.Reset(isolate, uint32arrayFrom);
  }

  ~AddonData() { isolate->AdjustAmountOfExternalAllocatedMemory(-sizeof(this)); }

  static inline AddonData * get(const v8::FunctionCallbackInfo<v8::Value> & info) {
    auto data = info.Data();
    if (data->IsExternal()) {
      auto result = static_cast<AddonData *>(v8::External::Cast(*data)->Value());
      if (AddonData::isActive(result)) {
        return result;
      }
    }
    return nullptr;
  }

  void initializationCompleted() {}

  void setStaticMethod(
    v8::Local<v8::External> addonDataExternal,
    v8::Local<v8::Object> recv,
    const char * name,
    v8::FunctionCallback callback) {
    v8::Isolate * isolate = this->isolate;
    v8::HandleScope scope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::FunctionTemplate> t = v8::FunctionTemplate::New(isolate, callback, addonDataExternal);
    v8::Local<v8::String> fn_name =
      v8::String::NewFromUtf8(isolate, name, v8::NewStringType::kInternalized).ToLocalChecked();
    t->SetClassName(fn_name);
    v8::Local<v8::Function> fn = t->GetFunction(context).ToLocalChecked();
    fn->SetName(fn_name);
    ignoreMaybeResult(recv->Set(context, fn_name, fn));
  }

  inline static bool isActive(const AddonData * addonData) {
    return addonData != nullptr &&
      AddonData::instances.find(const_cast<AddonData *>(addonData)) != AddonData::instances.end();
  }

 private:
  static std::mutex instancesMutex;
  static std::unordered_set<AddonData *> instances;

  static void AddonData_Cleanup(void * addonData) {
    std::lock_guard<std::mutex> lock(AddonData::instancesMutex);
    AddonData::instances.erase(static_cast<AddonData *>(addonData));
    delete static_cast<AddonData *>(addonData);
  }

  static void AddonData_Init(AddonData * addonData) {
    std::lock_guard<std::mutex> lock(AddonData::instancesMutex);
    AddonData::instances.insert(addonData);
    node::AddEnvironmentCleanupHook(addonData->isolate, AddonData::AddonData_Cleanup, addonData);
  }
};

std::mutex AddonData::instancesMutex;
std::unordered_set<AddonData *> AddonData::instances;

#endif
