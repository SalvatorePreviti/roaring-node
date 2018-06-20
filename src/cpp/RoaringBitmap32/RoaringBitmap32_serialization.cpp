#include "../v8utils.h"

#include "../RoaringBitmap32Iterator/RoaringBitmap32Iterator.h"
#include "RoaringBitmap32.h"

#define MAX_SERIALIZATION_ARRAY_SIZE_IN_BYTES 0x00FFFFFF

void RoaringBitmap32::getSerializationSizeInBytes(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
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
  v8::HandleScope scope(isolate);

  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());

  bool portable = info.Length() > 0 && info[0]->IsTrue();
  auto portablesize = roaring_bitmap_portable_size_in_bytes(&self->roaring);

  if (portable) {
    auto typedArray = TypedArrays::bufferAllocUnsafe(info.GetIsolate(), portablesize);
    const v8utils::TypedArrayContent<uint8_t> buf(isolate, typedArray);
    if (!buf.length || !buf.data)
      return v8utils::throwError("RoaringBitmap32::serialize - failed to allocate");

    roaring_bitmap_portable_serialize(&self->roaring, (char *)buf.data);
    info.GetReturnValue().Set(typedArray);
  } else {
    auto cardinality = roaring_bitmap_get_cardinality(&self->roaring);
    auto sizeasarray = cardinality * sizeof(uint32_t) + sizeof(uint32_t);

    if (portablesize < sizeasarray || sizeasarray >= MAX_SERIALIZATION_ARRAY_SIZE_IN_BYTES - 1) {
      auto typedArray = TypedArrays::bufferAllocUnsafe(isolate, portablesize + 1);
      const v8utils::TypedArrayContent<uint8_t> buf(isolate, typedArray);
      if (!buf.length || !buf.data)
        return v8utils::throwError("RoaringBitmap32::serialize - failed to allocate");

      buf.data[0] = SERIALIZATION_CONTAINER;
      roaring_bitmap_portable_serialize(&self->roaring, (char *)buf.data + 1);
      info.GetReturnValue().Set(typedArray);
    } else {
      auto typedArray = TypedArrays::bufferAllocUnsafe(isolate, (size_t)sizeasarray + 1);
      const v8utils::TypedArrayContent<uint8_t> buf(isolate, typedArray);
      if (!buf.length || !buf.data)
        return v8utils::throwError("RoaringBitmap32::serialize - failed to allocate");

      buf.data[0] = SERIALIZATION_ARRAY_UINT32;
      memcpy(buf.data + 1, &cardinality, sizeof(uint32_t));
      roaring_bitmap_to_uint32_array(&self->roaring, (uint32_t *)(buf.data + 1 + sizeof(uint32_t)));
      info.GetReturnValue().Set(typedArray);
    }
  }
}

void RoaringBitmap32::deserializeStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32::deserializeInner(info, true);
}

void RoaringBitmap32::deserialize(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32::deserializeInner(info, false);
}

void RoaringBitmap32::deserializeInner(const v8::FunctionCallbackInfo<v8::Value> & info, bool isStatic) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  if (info.Length() == 0 || (!info[0]->IsUint8Array() && !info[0]->IsInt8Array() && !info[0]->IsUint8ClampedArray()))
    return v8utils::throwTypeError("RoaringBitmap32::deserialize requires an argument of type Uint8Array or Buffer");

  RoaringBitmap32 * self = nullptr;

  if (isStatic) {
    v8::Local<v8::Function> cons = constructor.Get(isolate);

    auto resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
    if (resultMaybe.IsEmpty())
      return;

    auto result = resultMaybe.ToLocalChecked();

    info.GetReturnValue().Set(result);
    self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(result);
  } else {
    self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  }

  if (!self) {
    return v8utils::throwError("RoaringBitmap32::deserialize could not create RoaringBitmap32");
  }

  bool portable = info.Length() > 1 && info[1]->IsTrue();

  const v8utils::TypedArrayContent<uint8_t> typedArray(isolate, info[0]);
  auto bufLen = typedArray.length;
  if (bufLen == 0) {
    roaring_bitmap_t newRoaring;
    if (!ra_init(&newRoaring.high_low_container))
      return v8utils::throwError("RoaringBitmap32::deserialize - failed to initialize a new roaring container");
    ra_clear(&self->roaring.high_low_container);
    self->roaring.high_low_container = std::move(newRoaring.high_low_container);
    return;
  }

  const char * bufaschar = (const char *)typedArray.data;

  if (portable) {
    roaring_bitmap_t * newRoaring = roaring_bitmap_portable_deserialize_safe(bufaschar, bufLen);
    if (newRoaring == nullptr) {
      return v8utils::throwError("RoaringBitmap32::deserialize failed");
    }

    ra_clear(&self->roaring.high_low_container);
    self->roaring.high_low_container = std::move(newRoaring->high_low_container);
    return;
  }

  switch ((unsigned char)bufaschar[0]) {
    case SERIALIZATION_ARRAY_UINT32: {
      uint32_t card;
      memcpy(&card, bufaschar + 1, sizeof(uint32_t));

      if (card * sizeof(uint32_t) + sizeof(uint32_t) + 1 != bufLen) {
        return v8utils::throwError("RoaringBitmap32::deserialize corrupted data");
      }

      const uint32_t * elems = (const uint32_t *)(bufaschar + 1 + sizeof(uint32_t));
      roaring_bitmap_t * newRoaring = roaring_bitmap_of_ptr(card, elems);
      if (newRoaring == nullptr) {
        return v8utils::throwError("RoaringBitmap32::deserialize allocation failed");
      }

      ra_clear(&self->roaring.high_low_container);
      self->roaring.high_low_container = std::move(newRoaring->high_low_container);
      return;
    }
    case SERIALIZATION_CONTAINER: {
      roaring_bitmap_t * newRoaring = roaring_bitmap_portable_deserialize_safe(bufaschar + 1, bufLen - 1);
      if (newRoaring == nullptr) {
        return v8utils::throwError("RoaringBitmap32::deserialize failed");
      }

      ra_clear(&self->roaring.high_low_container);
      self->roaring.high_low_container = std::move(newRoaring->high_low_container);
      return;
    }
  }

  return v8utils::throwError("RoaringBitmap32::deserialize invalid data");
}