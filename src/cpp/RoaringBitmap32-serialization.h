#ifndef ROARING_NODE_ROARING_BITMAP32_SERIALIZATION_
#define ROARING_NODE_ROARING_BITMAP32_SERIALIZATION_

#include "RoaringBitmap32.h"
#include "serialization.h"
#include "async-workers.h"

void RoaringBitmap32_serialize(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  RoaringBitmapSerializer serializer;
  serializer.parseArguments(info);
  if (serializer.self) {
    const char * error = serializer.serialize();
    if (error) {
      return v8utils::throwError(isolate, error);
    }
    v8::Local<v8::Value> result;
    serializer.done(isolate, result);
    if (!result.IsEmpty()) {
      info.GetReturnValue().Set(result);
    }
  }
}

void RoaringBitmap32_serializeAsync(const v8::FunctionCallbackInfo<v8::Value> & info) {
  SerializeWorker * worker = new SerializeWorker(info, nullptr);
  info.GetReturnValue().Set(AsyncWorker::run(worker));
}

void RoaringBitmap32_unsafeFrozenViewStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  v8::Local<v8::Function> cons = globalAddonData.RoaringBitmap32_constructor.Get(isolate);

  v8::MaybeLocal<v8::Object> resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
  v8::Local<v8::Object> result;
  if (!resultMaybe.ToLocal(&result)) {
    return;
  }

  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(result, isolate);
  if (self == nullptr) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  self->replaceBitmapInstance(isolate, nullptr);

  if (info.Length() < 2) {
    return v8utils::throwError(isolate, "RoaringBitmap32::unsafeFrozenView expects a format and a buffer arguments");
  }

  int bufferArgIndex = 1;
  FrozenViewFormat format = tryParseFrozenViewFormat(info[0], isolate);
  if (format == FrozenViewFormat::INVALID) {
    bufferArgIndex = 0;
    format = tryParseFrozenViewFormat(info[1], isolate);
  }
  if (format == FrozenViewFormat::INVALID) {
    return v8utils::throwError(isolate, "RoaringBitmap32::unsafeFrozenView format argument is invalid");
  }

  v8utils::TypedArrayContent<uint8_t> storage(isolate, info[bufferArgIndex]);

  v8utils::TypedArrayContent<uint8_t> & frozenStorage = self->frozenStorage;

  if (
    !info[bufferArgIndex]->IsNullOrUndefined() &&
    !frozenStorage.set(isolate, info[bufferArgIndex]->ToObject(isolate->GetCurrentContext()))) {
    return v8utils::throwError(isolate, "RoaringBitmap32::unsafeFrozenView buffer argument was invalid");
  }

  self->frozenCounter = RoaringBitmap32::FROZEN_COUNTER_HARD_FROZEN;

  roaring_bitmap_t * bitmap = nullptr;

  switch (format) {
    case FrozenViewFormat::unsafe_frozen_croaring: {
      if (!is_pointer_aligned(storage.data, 32)) {
        return v8utils::throwError(
          isolate,
          "RoaringBitmap32::unsafeFrozenView with format unsafe_frozen_croaring requires the buffer to be 32 bytes aligned. "
          "You can use bufferAlignedAlloc, bufferAlignedAllocUnsafe or ensureBufferAligned exposed by the roaring library.");
      }

      bitmap =
        const_cast<roaring_bitmap_t *>(roaring_bitmap_frozen_view((const char *)frozenStorage.data, frozenStorage.length));
      break;
    }
    case FrozenViewFormat::unsafe_frozen_portable: {
      bitmap = roaring_bitmap_portable_deserialize_frozen((const char *)frozenStorage.data);
      break;
    }
    default: {
      return v8utils::throwError(isolate, "RoaringBitmap32::unsafeFrozenView format argument is invalid");
    }
  }

  if (!bitmap) {
    return v8utils::throwError(isolate, "RoaringBitmap32::unsafeFrozenView failed to deserialize the input");
  }

  self->replaceBitmapInstance(isolate, bitmap);

  info.GetReturnValue().Set(result);
}

void RoaringBitmap32_deserializeStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  v8::Local<v8::Function> cons = globalAddonData.RoaringBitmap32_constructor.Get(isolate);

  v8::MaybeLocal<v8::Object> resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
  v8::Local<v8::Object> result;
  if (!resultMaybe.ToLocal(&result)) {
    return;
  }

  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(result, isolate);
  if (self == nullptr) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  self->replaceBitmapInstance(isolate, nullptr);

  RoaringBitmapDeserializer deserializer;
  const char * error = deserializer.parseArguments(info, false);
  if (!error) {
    error = deserializer.deserialize();
  }
  if (error) {
    return v8utils::throwError(isolate, error);
  }

  deserializer.finalizeTargetBitmap(self);

  info.GetReturnValue().Set(result);
}

void RoaringBitmap32_deserialize(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  RoaringBitmapDeserializer deserializer;
  const char * error = deserializer.parseArguments(info, true);
  if (!error) {
    error = deserializer.deserialize();
  }
  if (error) {
    return v8utils::throwError(isolate, error);
  }

  deserializer.targetBitmap->replaceBitmapInstance(isolate, nullptr);

  deserializer.finalizeTargetBitmap(deserializer.targetBitmap);

  info.GetReturnValue().Set(info.Holder());
}

void RoaringBitmap32_deserializeStaticAsync(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  auto * worker = new DeserializeWorker(info, &globalAddonData);
  if (worker == nullptr) {
    return v8utils::throwError(isolate, "RoaringBitmap32 deserialization failed to allocate async worker");
  }

  if (info.Length() >= 3 && info[2]->IsFunction()) {
    worker->setCallback(info[2]);
  }

  v8::Local<v8::Value> returnValue = AsyncWorker::run(worker);
  info.GetReturnValue().Set(returnValue);
}

void RoaringBitmap32_deserializeParallelStaticAsync(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  auto * worker = new DeserializeParallelWorker(isolate, &globalAddonData);
  if (worker == nullptr) {
    return v8utils::throwError(isolate, "Failed to allocate async worker");
  }

  if (info.Length() >= 3 && info[2]->IsFunction()) {
    worker->setCallback(info[2]);
  }

  if (info.Length() < 2) {
    worker->setError("RoaringBitmap32::deserializeAsync - requires at least two arguments");
    return info.GetReturnValue().Set(AsyncWorker::run(worker));
  }

  if (!info[0]->IsArray()) {
    worker->setError("RoaringBitmap32::deserializeParallelAsync requires an array as first argument");
    return info.GetReturnValue().Set(AsyncWorker::run(worker));
  }

  auto array = v8::Local<v8::Array>::Cast(info[0]);
  uint32_t length = array->Length();

  if (length > 0x01FFFFFF) {
    worker->setError("RoaringBitmap32::deserializeParallelAsync - array too big");
    return info.GetReturnValue().Set(AsyncWorker::run(worker));
  }

  DeserializationFormat format = tryParseDeserializationFormat(info[1], isolate);
  if (format == DeserializationFormat::INVALID) {
    worker->setError("RoaringBitmap32::deserializeAsync - second argument must be a valid deserialization format");
    return info.GetReturnValue().Set(AsyncWorker::run(worker));
  }

  RoaringBitmapDeserializer * items = length ? new RoaringBitmapDeserializer[length]() : nullptr;
  if (items == nullptr && length != 0) {
    worker->setError("RoaringBitmap32::deserializeParallelAsync - failed to allocate array of deserializers");
    return info.GetReturnValue().Set(AsyncWorker::run(worker));
  }

  worker->items = items;
  worker->loopCount = length;

  auto context = isolate->GetCurrentContext();
  for (uint32_t i = 0; i != length; ++i) {
    const char * err = items[i].setOutput(isolate, array->Get(context, i), format);
    if (err != nullptr) {
      worker->setError(err);
      return info.GetReturnValue().Set(AsyncWorker::run(worker));
    }
  }

  info.GetReturnValue().Set(AsyncWorker::run(worker));
}

void RoaringBitmap32_getSerializationSizeInBytes(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (self == nullptr) {
    return info.GetReturnValue().Set(0U);
  }

  SerializationFormat format =
    info.Length() > 0 ? tryParseSerializationFormat(info[0], isolate) : SerializationFormat::INVALID;

  if (format == SerializationFormat::INVALID) {
    return v8utils::throwError(
      info.GetIsolate(), "RoaringBitmap32::getSerializationSizeInBytes format argument was invalid");
  }

  switch (format) {
    case SerializationFormat::croaring: {
      auto cardinality = self->getSize();
      auto sizeasarray = cardinality * sizeof(uint32_t) + sizeof(uint32_t);
      auto portablesize = roaring_bitmap_portable_size_in_bytes(self->roaring);
      if (portablesize < sizeasarray || sizeasarray >= MAX_SERIALIZATION_ARRAY_SIZE_IN_BYTES - 1) {
        return info.GetReturnValue().Set((double)(portablesize) + 1);
      }
      return info.GetReturnValue().Set((double)(sizeasarray) + 1);
    }

    case SerializationFormat::portable: {
      return info.GetReturnValue().Set((double)(roaring_bitmap_portable_size_in_bytes(self->roaring)));
    }

    case SerializationFormat::unsafe_frozen_croaring: {
      return info.GetReturnValue().Set((double)(roaring_bitmap_frozen_size_in_bytes(self->roaring)));
    }

    default: {
      return v8utils::throwError(
        info.GetIsolate(), "RoaringBitmap32::getSerializationSizeInBytes format argument was invalid");
    }
  }
}

#endif  // ROARING_NODE_ROARING_BITMAP32_SERIALIZATION_
