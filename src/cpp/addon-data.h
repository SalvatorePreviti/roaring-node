#ifndef ROARING_NODE_ADDON_DATA_
#define ROARING_NODE_ADDON_DATA_

#include "includes.h"
#include "memory.h"
#include "addon-strings.h"
#include <mutex>

template <typename T>
inline void ignoreMaybeResult(v8::Maybe<T>) {}

template <typename T>
inline void ignoreMaybeResult(v8::MaybeLocal<T>) {}

class AddonData final {
 public:
  v8::Isolate * isolate;

  AddonDataStrings strings;

  v8::Global<v8::Object> Buffer;
  v8::Global<v8::Object> Uint32Array;
  v8::Global<v8::Function> Uint32Array_from;
  v8::Global<v8::Function> Buffer_from;

  std::atomic<uint64_t> RoaringBitmap32_instances;
  std::atomic<uint32_t> activeAsyncWorkers;
  std::atomic<bool> shuttingDown;

  v8::Global<v8::FunctionTemplate> RoaringBitmap32_constructorTemplate;
  v8::Global<v8::Function> RoaringBitmap32_constructor;

  v8::Global<v8::FunctionTemplate> RoaringBitmap32BufferedIterator_constructorTemplate;
  v8::Global<v8::Function> RoaringBitmap32BufferedIterator_constructor;

  v8::Global<v8::External> external;

  inline explicit AddonData(v8::Isolate * isolate) :
    isolate(isolate), strings(isolate), RoaringBitmap32_instances(0), activeAsyncWorkers(0), shuttingDown(false) {
    _gcaware_adjustAllocatedMemory(this->isolate, sizeof(AddonData));
    {
      std::lock_guard<std::mutex> lock(addonDataMapMutex());
      addonDataMap()[isolate] = this;
    }
  }

  inline ~AddonData() {
    shuttingDown.store(true, std::memory_order_release);
    waitForAsyncWorkersToFinish();
    Buffer.Reset();
    Uint32Array.Reset();
    Uint32Array_from.Reset();
    Buffer_from.Reset();
    RoaringBitmap32_constructorTemplate.Reset();
    RoaringBitmap32_constructor.Reset();
    RoaringBitmap32BufferedIterator_constructorTemplate.Reset();
    RoaringBitmap32BufferedIterator_constructor.Reset();
    external.Reset();
    {
      std::lock_guard<std::mutex> lock(addonDataMapMutex());
      addonDataMap().erase(this->isolate);
    }
    _gcaware_adjustAllocatedMemory(this->isolate, -static_cast<int64_t>(sizeof(AddonData)));
  }

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

  static inline AddonData * FromIsolate(v8::Isolate * isolate) {
    if (isolate == nullptr) {
      return nullptr;
    }
    std::lock_guard<std::mutex> lock(addonDataMapMutex());
    auto & map = addonDataMap();
    auto it = map.find(isolate);
    return it != map.end() ? it->second : nullptr;
  }

  inline void initialize() {
    v8::Isolate * isolate = this->isolate;
    if (isolate == nullptr) {
      return;
    }

    external.Reset(isolate, v8::External::New(isolate, this));

    auto context = isolate->GetCurrentContext();

    auto global = context->Global();

    auto uint32Array = global->Get(context, NEW_LITERAL_V8_STRING(isolate, "Uint32Array", v8::NewStringType::kInternalized))
                         .ToLocalChecked()
                         ->ToObject(context)
                         .ToLocalChecked();

    auto buffer = global->Get(context, NEW_LITERAL_V8_STRING(isolate, "Buffer", v8::NewStringType::kInternalized))
                    .ToLocalChecked()
                    .As<v8::Object>();

    this->Buffer.Reset(isolate, buffer);

    this->Buffer_from.Reset(
      isolate,
      buffer->Get(context, NEW_LITERAL_V8_STRING(isolate, "from", v8::NewStringType::kInternalized))
        .ToLocalChecked()
        .As<v8::Function>());

    this->Uint32Array.Reset(isolate, uint32Array);

    this->Uint32Array_from.Reset(
      isolate,
      v8::Local<v8::Function>::Cast(
        uint32Array->Get(context, NEW_LITERAL_V8_STRING(isolate, "from", v8::NewStringType::kInternalized))
          .ToLocalChecked()));
  }

  inline void setMethod(v8::Local<v8::Object> recv, const char * name, v8::FunctionCallback callback) {
    if (recv.IsEmpty() || name == nullptr || callback == nullptr) {
      return;
    }

    v8::Isolate * isolate = this->isolate;
    if (isolate == nullptr) {
      return;
    }

    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate, callback, this->external.Get(isolate));
    v8::Local<v8::Function> fn = tpl->GetFunction(context).ToLocalChecked();
    v8::Local<v8::String> fnName = v8::String::NewFromUtf8(isolate, name, v8::NewStringType::kInternalized).ToLocalChecked();
    fn->SetName(fnName);
    ignoreMaybeResult(recv->Set(context, fnName, fn));
  }

  inline bool tryEnterAsyncWorker() {
    if (isShuttingDown()) {
      return false;
    }
    activeAsyncWorkers.fetch_add(1, std::memory_order_acq_rel);
    if (isShuttingDown()) {
      activeAsyncWorkers.fetch_sub(1, std::memory_order_acq_rel);
      return false;
    }
    return true;
  }

  inline void leaveAsyncWorker() { activeAsyncWorkers.fetch_sub(1, std::memory_order_acq_rel); }

  inline bool isShuttingDown() const { return shuttingDown.load(std::memory_order_acquire); }

  inline void waitForAsyncWorkersToFinish() {
    uint32_t delayMicros = 50;
    while (activeAsyncWorkers.load(std::memory_order_acquire) != 0) {
      std::this_thread::sleep_for(std::chrono::microseconds(delayMicros));
      delayMicros = delayMicros >= 5000 ? 5000 : delayMicros * 2;
    }
  }

 private:
  static inline std::unordered_map<v8::Isolate *, AddonData *> & addonDataMap() {
    static std::unordered_map<v8::Isolate *, AddonData *> map;
    return map;
  }

  static inline std::mutex & addonDataMapMutex() {
    static std::mutex mutex;
    return mutex;
  }
};

#endif
