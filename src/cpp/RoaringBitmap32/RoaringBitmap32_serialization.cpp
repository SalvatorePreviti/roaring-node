#include "RoaringBitmap32.h"

#define MAX_SERIALIZATION_ARRAY_SIZE_IN_BYTES 0x00FFFFFF

void RoaringBitmap32::getSerializationSizeInBytes(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  bool portable = info.Length() > 0 && info[0]->IsTrue();

  auto portablesize = roaring_bitmap_portable_size_in_bytes(&self->roaring);

  if (portable) {
    return info.GetReturnValue().Set((double)portablesize);
  }

  auto cardinality = roaring_bitmap_get_cardinality(&self->roaring);
  auto sizeasarray = cardinality * sizeof(uint32_t) + sizeof(uint32_t);

  if (portablesize < sizeasarray || sizeasarray >= MAX_SERIALIZATION_ARRAY_SIZE_IN_BYTES - 1) {
    return info.GetReturnValue().Set((double)(portablesize + 1));
  }

  return info.GetReturnValue().Set((double)(sizeasarray + 1));
}

void RoaringBitmap32::serialize(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::EscapableHandleScope scope(isolate);

  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());

  bool portable = info.Length() > 0 && info[0]->IsTrue();
  auto portablesize = roaring_bitmap_portable_size_in_bytes(&self->roaring);

  if (portable) {
    auto typedArray = JSTypes::bufferAllocUnsafe(info.GetIsolate(), portablesize);
    const v8utils::TypedArrayContent<uint8_t> buf(typedArray);
    if (!buf.length || !buf.data)
      return v8utils::throwError("RoaringBitmap32::serialize - failed to allocate");

    roaring_bitmap_portable_serialize(&self->roaring, (char *)buf.data);
    info.GetReturnValue().Set(scope.Escape(typedArray));
  } else {
    auto cardinality = roaring_bitmap_get_cardinality(&self->roaring);
    auto sizeasarray = cardinality * sizeof(uint32_t) + sizeof(uint32_t);

    if (portablesize < sizeasarray || sizeasarray >= MAX_SERIALIZATION_ARRAY_SIZE_IN_BYTES - 1) {
      auto typedArray = JSTypes::bufferAllocUnsafe(isolate, portablesize + 1);
      const v8utils::TypedArrayContent<uint8_t> buf(typedArray);
      if (!buf.length || !buf.data)
        return v8utils::throwError("RoaringBitmap32::serialize - failed to allocate");

      buf.data[0] = SERIALIZATION_CONTAINER;
      roaring_bitmap_portable_serialize(&self->roaring, (char *)buf.data + 1);
      info.GetReturnValue().Set(scope.Escape(typedArray));
    } else {
      auto typedArray = JSTypes::bufferAllocUnsafe(isolate, (size_t)sizeasarray + 1);
      const v8utils::TypedArrayContent<uint8_t> buf(typedArray);
      if (!buf.length || !buf.data)
        return v8utils::throwError("RoaringBitmap32::serialize - failed to allocate");

      buf.data[0] = SERIALIZATION_ARRAY_UINT32;
      memcpy(buf.data + 1, &cardinality, sizeof(uint32_t));
      roaring_bitmap_to_uint32_array(&self->roaring, (uint32_t *)(buf.data + 1 + sizeof(uint32_t)));
      info.GetReturnValue().Set(scope.Escape(typedArray));
    }
  }
}

const char * RoaringBitmap32::doDeserialize(const v8utils::TypedArrayContent<uint8_t> & typedArray, bool portable, roaring_bitmap_t & bitmap) {
  bitmap = *((roaring_bitmap_t *)&RoaringBitmap32::roaring_bitmap_zero);

  auto bufLen = typedArray.length;
  const char * bufaschar = (const char *)typedArray.data;

  if (bufLen == 0 || !bufaschar) {
    return nullptr;
  }

  if (portable) {
    size_t bytesread;
    bool ok = ra_portable_deserialize(&bitmap.high_low_container, bufaschar, bufLen, &bytesread);
    if (!ok) {
      return "RoaringBitmap32::deserialize - portable deserialization failed";
    }
    return nullptr;
  }

  switch ((unsigned char)bufaschar[0]) {
    case SERIALIZATION_ARRAY_UINT32: {
      uint32_t card;
      memcpy(&card, bufaschar + 1, sizeof(uint32_t));

      if (card * sizeof(uint32_t) + sizeof(uint32_t) + 1 != bufLen) {
        return "RoaringBitmap32::deserialize - corrupted data, wrong cardinality header";
      }

      ra_clear(&bitmap.high_low_container);
      bitmap.high_low_container = ((roaring_bitmap_t *)&RoaringBitmap32::roaring_bitmap_zero)->high_low_container;

      const uint32_t * elems = (const uint32_t *)(bufaschar + 1 + sizeof(uint32_t));
      roaring_bitmap_add_many(&bitmap, card, elems);
      roaring_bitmap_run_optimize(&bitmap);
      roaring_bitmap_shrink_to_fit(&bitmap);
      return nullptr;
    }

    case SERIALIZATION_CONTAINER: {
      size_t bytesread;
      bool ok = ra_portable_deserialize(&bitmap.high_low_container, bufaschar + 1, bufLen - 1, &bytesread);
      if (!ok) {
        return "RoaringBitmap32::deserialize - deserialization failed";
      }
      return nullptr;
    }
  }

  return "RoaringBitmap32::deserialize - invalid portable header byte";
}

void RoaringBitmap32::deserializeStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::EscapableHandleScope scope(isolate);

  if (info.Length() == 0 || (!info[0]->IsUint8Array() && !info[0]->IsInt8Array() && !info[0]->IsUint8ClampedArray()))
    return v8utils::throwTypeError("RoaringBitmap32::deserialize requires an argument of type Uint8Array or Buffer");

  const v8utils::TypedArrayContent<uint8_t> typedArray(info[0]);

  roaring_bitmap_t bitmap;
  const char * error = doDeserialize(typedArray, info.Length() > 1 && info[1]->IsTrue(), bitmap);
  if (error != nullptr) {
    return v8utils::throwError(error);
  }

  v8::Local<v8::Function> cons = constructor.Get(isolate);

  v8::MaybeLocal<v8::Object> resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
  if (resultMaybe.IsEmpty()) {
    ra_clear(&bitmap.high_low_container);
    bitmap.high_low_container = ((roaring_bitmap_t *)&RoaringBitmap32::roaring_bitmap_zero)->high_low_container;
    return;
  }

  v8::Local<v8::Object> result = resultMaybe.ToLocalChecked();

  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(result);

  self->roaring = std::move(bitmap);

  info.GetReturnValue().Set(scope.Escape(result));
}

void RoaringBitmap32::deserialize(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::EscapableHandleScope scope(isolate);

  if (info.Length() == 0 || (!info[0]->IsUint8Array() && !info[0]->IsInt8Array() && !info[0]->IsUint8ClampedArray()))
    return v8utils::throwTypeError("RoaringBitmap32::deserialize requires an argument of type Uint8Array or Buffer");

  auto holder = info.Holder();

  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(holder);
  self->invalidate();

  const v8utils::TypedArrayContent<uint8_t> typedArray(info[0]);
  roaring_bitmap_t bitmap;
  const char * error = doDeserialize(typedArray, info.Length() > 1 && info[1]->IsTrue(), bitmap);
  if (error != nullptr) {
    return v8utils::throwError(error);
  }

  self->roaring.high_low_container = std::move(bitmap.high_low_container);

  info.GetReturnValue().Set(scope.Escape(holder));
}

class DeserializeWorker : public RoaringBitmap32FactoryAsyncWorker {
 public:
  v8::Persistent<v8::Value> bufferPersistent;
  v8utils::TypedArrayContent<uint8_t> buffer;
  bool portable;

  DeserializeWorker(v8::Isolate * isolate) : RoaringBitmap32FactoryAsyncWorker(isolate), portable(false) {
  }

  virtual ~DeserializeWorker() {
    bufferPersistent.Reset();
  }

  bool setBuffer(v8::Local<v8::Value> buffer) {
    if (buffer.IsEmpty() || (!buffer->IsUint8Array() && !buffer->IsInt8Array() && !buffer->IsUint8ClampedArray())) {
      return false;
    }
    bufferPersistent.Reset(isolate, buffer);
    if (!this->buffer.set(buffer)) {
      return false;
    }
    return true;
  }

 protected:
  virtual void work() {
    this->setError(RoaringBitmap32::doDeserialize(buffer, portable, bitmap));
  }
};

void RoaringBitmap32::deserializeStaticAsync(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  v8::EscapableHandleScope scope(isolate);

  DeserializeWorker * worker = new DeserializeWorker(isolate);
  if (worker == nullptr) {
    return v8utils::throwError("RoaringBitmap32::deserializeAsync - Failed to allocate async worker");
  }

  if (info.Length() <= 0 || !worker->setBuffer(info[0])) {
    delete worker;
    return v8utils::throwTypeError("RoaringBitmap32::deserializeAsync - first argument must be Uint8Array or Buffer");
  }

  if (info.Length() >= 2) {
    if (info[1]->IsFunction()) {
      worker->setCallback(info[1]);
      if (info.Length() >= 3 && info[2]->IsTrue()) {
        worker->portable = true;
      }
    } else {
      if (info[1]->IsTrue()) {
        worker->portable = true;
      }
      if (info.Length() >= 3 && info[2]->IsFunction()) {
        worker->setCallback(info[2]);
      }
    }
  }

  v8::Local<v8::Value> returnValue = v8utils::AsyncWorker::run(worker);
  info.GetReturnValue().Set(scope.Escape(returnValue));
}

class DeserializeParallelWorkerItem {
 public:
  roaring_bitmap_t bitmap;
  v8::Persistent<v8::Value> bufferPersistent;
  v8utils::TypedArrayContent<uint8_t> buffer;
  bool moved;

  DeserializeParallelWorkerItem() : bitmap(*((roaring_bitmap_t *)&RoaringBitmap32::roaring_bitmap_zero)), moved(false) {
  }

  ~DeserializeParallelWorkerItem() {
    bufferPersistent.Reset();
    if (!moved) {
      ra_clear(&bitmap.high_low_container);
    }
  }

  bool setBuffer(v8::Isolate * isolate, v8::Local<v8::Value> buffer) {
    if (buffer.IsEmpty() || (!buffer->IsUint8Array() && !buffer->IsInt8Array() && !buffer->IsUint8ClampedArray())) {
      return false;
    }
    if (!this->buffer.set(buffer)) {
      return false;
    }
    return true;
  }
};

class DeserializeParallelWorker : public v8utils::ParallelAsyncWorker {
 public:
  v8::Persistent<v8::Value> bufferPersistent;

  bool portable;
  DeserializeParallelWorkerItem * items;

  DeserializeParallelWorker(v8::Isolate * isolate) : v8utils::ParallelAsyncWorker(isolate), portable(false), items(nullptr) {
  }

  virtual ~DeserializeParallelWorker() {
    if (items != nullptr) {
      delete[] items;
    }
  }

 protected:
  virtual void parallelWork(uint32_t index) {
    DeserializeParallelWorkerItem & item = items[index];
    const char * error = RoaringBitmap32::doDeserialize(item.buffer, portable, item.bitmap);
    if (error != nullptr) {
      this->setError(error);
    }
  }

  virtual v8::Local<v8::Value> done() {
    v8::Local<v8::Function> cons = RoaringBitmap32::constructor.Get(isolate);

    const uint32_t itemsCount = this->loopCount;
    DeserializeParallelWorkerItem * items = this->items;

    v8::MaybeLocal<v8::Array> resultArrayMaybe = v8::Array::New(isolate, itemsCount);
    if (resultArrayMaybe.IsEmpty()) {
      return empty();
    }

    v8::Local<v8::Array> resultArray = resultArrayMaybe.ToLocalChecked();
    v8::Local<v8::Context> currentContext = isolate->GetCurrentContext();

    for (uint32_t i = 0; i != itemsCount; ++i) {
      v8::MaybeLocal<v8::Object> instanceMaybe = cons->NewInstance(currentContext, 0, nullptr);
      if (instanceMaybe.IsEmpty()) {
        return empty();
      }

      v8::Local<v8::Object> instance = instanceMaybe.ToLocalChecked();

      RoaringBitmap32 * unwrapped = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(instance);

      DeserializeParallelWorkerItem & item = items[i];

      unwrapped->roaring = std::move(item.bitmap);
      item.moved = true;

      resultArray->Set(i, instance);
    }

    return resultArray;
  }
};

void RoaringBitmap32::deserializeParallelStaticAsync(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  v8::EscapableHandleScope scope(isolate);

  if (info.Length() < 1 || !info[0]->IsArray()) {
    return v8utils::throwTypeError("RoaringBitmap32::deserializeParallelAsync requires an array as first argument");
  }

  v8::Handle<v8::Array> array = v8::Local<v8::Array>::Cast(info[0]);

  uint32_t length = array->Length();

  if (length > 0x01FFFFFF) {
    return v8utils::throwTypeError("RoaringBitmap32::deserializeParallelAsync - array too big");
  }

  DeserializeParallelWorker * worker = new DeserializeParallelWorker(isolate);
  if (worker == nullptr) {
    return v8utils::throwError("Failed to allocate async worker");
  }

  DeserializeParallelWorkerItem * items = length ? new DeserializeParallelWorkerItem[length]() : nullptr;
  if (items == nullptr && length != 0) {
    delete worker;
    return v8utils::throwError("Failed to allocate async worker memory");
  }
  worker->items = items;
  worker->loopCount = length;

  for (uint32_t i = 0; i != length; ++i) {
    v8::Local<v8::Value> v = array->Get(i);
    if (!v->IsNullOrUndefined() && !items[i].setBuffer(isolate, v)) {
      delete worker;
      return v8utils::throwTypeError("RoaringBitmap32::deserializeParallelAsync - Array must contains only Uint8Array or Buffer elements");
    }
  }

  if (info.Length() >= 2) {
    if (info[1]->IsFunction()) {
      worker->setCallback(info[1]);
      if (info.Length() >= 3 && info[2]->IsTrue()) {
        worker->portable = true;
      }
    } else {
      if (info[1]->IsTrue()) {
        worker->portable = true;
      }
      if (info.Length() >= 3 && info[2]->IsFunction()) {
        worker->setCallback(info[2]);
      }
    }
  }

  v8::Local<v8::Value> returnValue = v8utils::AsyncWorker::run(worker);
  info.GetReturnValue().Set(scope.Escape(returnValue));
}
