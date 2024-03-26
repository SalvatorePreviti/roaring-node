#ifndef ROARING_NODE_ROARING_BITMAP_32_BUFFERED_ITERATOR_
#define ROARING_NODE_ROARING_BITMAP_32_BUFFERED_ITERATOR_

#include "RoaringBitmap32.h"

class RoaringBitmap32BufferedIterator final {
 public:
  static constexpr const uint64_t OBJECT_TOKEN = 0x21524F4152490000;

  enum { allocatedMemoryDelta = 1024 };

  roaring_uint32_iterator_t it;
  AddonData * const addonData;
  bool reversed;
  RoaringBitmap32 * bitmapInstance;
  int64_t bitmapVersion;
  v8utils::TypedArrayContent<uint32_t> bufferContent;

  v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> bitmap;
  v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> persistent;

  explicit RoaringBitmap32BufferedIterator(AddonData * addonData, bool reversed) :
    addonData(addonData), reversed(reversed), bitmapInstance(nullptr) {
    this->it.parent = nullptr;
    this->it.has_value = false;
    gcaware_addAllocatedMemory(sizeof(RoaringBitmap32BufferedIterator));
  }

  ~RoaringBitmap32BufferedIterator() {
    this->destroy();
    gcaware_removeAllocatedMemory(sizeof(RoaringBitmap32BufferedIterator));
  }

  inline uint32_t _fill() {
    uint32_t n;
    if (this->reversed) {
      size_t size = this->bufferContent.length;
      auto * data = this->bufferContent.data;
      for (n = 0; n < size && this->it.has_value; ++n) {
        data[n] = this->it.current_value;
        roaring_uint32_iterator_previous(&this->it);
      }
    } else {
      n = roaring_uint32_iterator_read(&this->it, this->bufferContent.data, this->bufferContent.length);
    }
    if (n == 0) {
      this->bitmapInstance = nullptr;
      this->bufferContent.reset();
      this->bitmap.Reset();
    }
    return n;
  }

 private:
  void destroy() {
    this->bitmap.Reset();
    if (!this->persistent.IsEmpty()) {
      this->persistent.ClearWeak();
      this->persistent.Reset();
    }
  }
};

void RoaringBitmap32BufferedIterator_fill(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();

  RoaringBitmap32BufferedIterator * instance =
    ObjectWrap::TryUnwrap<RoaringBitmap32BufferedIterator>(info.Holder(), isolate);

  RoaringBitmap32 * bitmapInstance = instance ? instance->bitmapInstance : nullptr;

  if (bitmapInstance == nullptr) {
    return info.GetReturnValue().Set(0U);
  }

  if (bitmapInstance->getVersion() != instance->bitmapVersion) {
    return v8utils::throwError(isolate, "RoaringBitmap32 iterator - bitmap changed while iterating");
  }

  return info.GetReturnValue().Set(instance->_fill());
}

void RoaringBitmap32BufferedIterator_WeakCallback(v8::WeakCallbackInfo<RoaringBitmap32BufferedIterator> const & info) {
  RoaringBitmap32BufferedIterator * p = info.GetParameter();
  if (p != nullptr) {
    delete p;
  }
}

void RoaringBitmap32BufferedIterator_New(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();

  if (!info.IsConstructCall()) {
    return v8utils::throwTypeError(isolate, "RoaringBitmap32BufferedIterator::ctor - needs to be called with new");
  }

  auto holder = info.Holder();

  if (info.Length() < 2) {
    return v8utils::throwTypeError(isolate, "RoaringBitmap32BufferedIterator::ctor - needs 2 or 3 arguments");
  }

  AddonData * addonData = AddonData::get(info);
  if (addonData == nullptr) {
    return v8utils::throwTypeError(isolate, ERROR_INVALID_OBJECT);
  }

  RoaringBitmap32 * bitmapInstance = ObjectWrap::TryUnwrap<RoaringBitmap32>(info[0], isolate);
  if (bitmapInstance == nullptr) {
    return v8utils::throwTypeError(
      isolate, "RoaringBitmap32BufferedIterator::ctor - first argument must be of type RoaringBitmap32");
  }

  bool reversed = info.Length() > 2 && info[2]->BooleanValue(isolate);

  auto bufferObject = info[1];

  if (!bufferObject->IsUint32Array()) {
    return v8utils::throwTypeError(
      isolate, "RoaringBitmap32BufferedIterator::ctor - second argument must be of type Uint32Array");
  }

  const v8utils::TypedArrayContent<uint32_t> bufferContent(isolate, bufferObject);
  if (!bufferContent.data || bufferContent.length < 1) {
    return v8utils::throwTypeError(isolate, "RoaringBitmap32BufferedIterator::ctor - invalid Uint32Array buffer");
  }

  auto * instance = new RoaringBitmap32BufferedIterator(addonData, reversed);
  if (instance == nullptr) {
    return v8utils::throwError(isolate, "RoaringBitmap32BufferedIterator::ctor - allocation failed");
  }

  int indices[2] = {0, 1};
  void * values[2] = {instance, (void *)(RoaringBitmap32BufferedIterator::OBJECT_TOKEN)};
  holder->SetAlignedPointerInInternalFields(2, indices, values);

  info.GetReturnValue().Set(holder);

  instance->persistent.Reset(isolate, holder);
  instance->persistent.SetWeak(instance, RoaringBitmap32BufferedIterator_WeakCallback, v8::WeakCallbackType::kParameter);

  auto context = isolate->GetCurrentContext();

  instance->bitmapInstance = bitmapInstance;
  instance->bitmapVersion = bitmapInstance->getVersion();

  auto a0Maybe = info[0]->ToObject(context);
  v8::Local<v8::Object> a0;
  if (!a0Maybe.ToLocal(&a0)) {
    return v8utils::throwError(isolate, "RoaringBitmap32BufferedIterator::ctor - allocation failed");
  }
  instance->bitmap.Reset(isolate, a0);

  auto a1Maybe = bufferObject->ToObject(context);
  v8::Local<v8::Object> a1;
  if (!a1Maybe.ToLocal(&a1)) {
    return v8utils::throwError(isolate, "RoaringBitmap32BufferedIterator::ctor - allocation failed");
  }

  instance->bufferContent.set(isolate, bufferObject);

  if (reversed) {
    roaring_iterator_init_last(bitmapInstance->roaring, &instance->it);
  } else {
    roaring_iterator_init(bitmapInstance->roaring, &instance->it);
  }

  uint32_t n = instance->_fill();

  if (holder->Set(context, addonData->strings.n.Get(isolate), v8::Uint32::NewFromUnsigned(isolate, n)).IsNothing()) {
    return v8utils::throwError(isolate, "RoaringBitmap32BufferedIterator::ctor - instantiation failed");
  }
}

void RoaringBitmap32BufferedIterator_Init(v8::Local<v8::Object> exports, AddonData * addonData) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();

  v8::Local<v8::FunctionTemplate> ctor =
    v8::FunctionTemplate::New(isolate, RoaringBitmap32BufferedIterator_New, addonData->persistent.Get(isolate));

  ctor->SetClassName(addonData->strings.RoaringBitmap32BufferedIterator.Get(isolate));
  ctor->InstanceTemplate()->SetInternalFieldCount(2);

  addonData->RoaringBitmap32BufferedIterator_constructorTemplate.Reset(isolate, ctor);

  NODE_SET_PROTOTYPE_METHOD(ctor, "fill", RoaringBitmap32BufferedIterator_fill);

  auto ctorFunctionMaybe = ctor->GetFunction(isolate->GetCurrentContext());
  v8::Local<v8::Function> ctorFunction;

  if (!ctorFunctionMaybe.ToLocal(&ctorFunction)) {
    return v8utils::throwError(isolate, "Failed to instantiate RoaringBitmap32BufferedIterator");
  }

  addonData->RoaringBitmap32BufferedIterator_constructor.Reset(isolate, ctorFunction);

  ignoreMaybeResult(exports->Set(
    isolate->GetCurrentContext(), addonData->strings.RoaringBitmap32BufferedIterator.Get(isolate), ctorFunction));
}

#endif  // ROARING_NODE_ROARING_BITMAP_32_BUFFERED_ITERATOR_
