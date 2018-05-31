#include "../v8utils.h"

#include "../RoaringBitmap32Iterator/RoaringBitmap32Iterator.h"
#include "../TypedArrays.h"
#include "RoaringBitmap32.h"

#define MAX_SERIALIZATION_ARRAY_SIZE_IN_BYTES 0x00FFFFFF

void RoaringBitmap32::getSerializationSizeInBytes(const Nan::FunctionCallbackInfo<v8::Value> & info) {
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

void RoaringBitmap32::serialize(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());

  bool portable = info.Length() > 0 && info[0]->IsTrue();
  auto portablesize = roaring_bitmap_portable_size_in_bytes(&self->roaring);

  if (portable) {
    auto typedArray = TypedArrays::bufferAllocUnsafe(info.GetIsolate(), portablesize);
    Nan::TypedArrayContents<uint8_t> buf(typedArray);
    if (!buf.length() || !*buf)
      return Nan::ThrowError(Nan::New("RoaringBitmap32::serialize - failed to allocate").ToLocalChecked());

    roaring_bitmap_portable_serialize(&self->roaring, (char *)*buf);
    info.GetReturnValue().Set(typedArray);
  } else {
    auto cardinality = roaring_bitmap_get_cardinality(&self->roaring);
    auto sizeasarray = cardinality * sizeof(uint32_t) + sizeof(uint32_t);

    if (portablesize < sizeasarray || sizeasarray >= MAX_SERIALIZATION_ARRAY_SIZE_IN_BYTES - 1) {
      auto typedArray = TypedArrays::bufferAllocUnsafe(info.GetIsolate(), portablesize + 1);
      Nan::TypedArrayContents<uint8_t> buf(typedArray);
      if (!buf.length() || !*buf)
        return Nan::ThrowError(Nan::New("RoaringBitmap32::serialize - failed to allocate").ToLocalChecked());

      (*buf)[0] = SERIALIZATION_CONTAINER;
      roaring_bitmap_portable_serialize(&self->roaring, (char *)*buf + 1);
      info.GetReturnValue().Set(typedArray);
    } else {
      auto typedArray = TypedArrays::bufferAllocUnsafe(info.GetIsolate(), sizeasarray + 1);
      Nan::TypedArrayContents<uint8_t> buf(typedArray);
      if (!buf.length() || !*buf)
        return Nan::ThrowError(Nan::New("RoaringBitmap32::serialize - failed to allocate").ToLocalChecked());

      (*buf)[0] = SERIALIZATION_ARRAY_UINT32;
      memcpy(*buf + 1, &cardinality, sizeof(uint32_t));
      roaring_bitmap_to_uint32_array(&self->roaring, (uint32_t *)(*buf + 1 + sizeof(uint32_t)));
      info.GetReturnValue().Set(typedArray);
    }
  }
}

void RoaringBitmap32::deserializeStatic(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32::deserializeInner(info, true);
}

void RoaringBitmap32::deserialize(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32::deserializeInner(info, false);
}

void RoaringBitmap32::deserializeInner(const Nan::FunctionCallbackInfo<v8::Value> & info, bool isStatic) {
  if (info.Length() == 0 || (!info[0]->IsUint8Array() && !info[0]->IsInt8Array() && !info[0]->IsUint8ClampedArray()))
    return Nan::ThrowTypeError(Nan::New("RoaringBitmap32::deserialize requires an argument of type Uint8Array or Buffer").ToLocalChecked());

  RoaringBitmap32 * self = nullptr;

  if (isStatic) {
    v8::Local<v8::Function> cons = Nan::New(constructor);
    v8::Local<v8::Value> argv[0] = {};

    auto resultMaybe = Nan::NewInstance(cons, 0, argv);
    if (resultMaybe.IsEmpty())
      return;

    auto result = resultMaybe.ToLocalChecked();

    info.GetReturnValue().Set(result);
    self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(result);
  } else {
    self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  }

  if (!self) {
    return Nan::ThrowError(Nan::New("RoaringBitmap32::deserialize could not create RoaringBitmap32").ToLocalChecked());
  }

  bool portable = info.Length() > 1 && info[1]->IsTrue();

  Nan::TypedArrayContents<uint8_t> typedArray(info[0]);
  auto bufLen = typedArray.length();
  if (bufLen == 0) {
    roaring_bitmap_t newRoaring;
    if (!ra_init(&newRoaring.high_low_container))
      return Nan::ThrowError(Nan::New("RoaringBitmap32::deserialize - failed to initialize a new roaring container").ToLocalChecked());
    ra_clear(&self->roaring.high_low_container);
    self->roaring.high_low_container = std::move(newRoaring.high_low_container);
    return;
  }

  const char * bufaschar = (const char *)*typedArray;

  if (portable) {
    roaring_bitmap_t * newRoaring = roaring_bitmap_portable_deserialize_safe(bufaschar, bufLen);
    if (newRoaring == nullptr) {
      return Nan::ThrowError(Nan::New("RoaringBitmap32::deserialize failed").ToLocalChecked());
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
        return Nan::ThrowError(Nan::New("RoaringBitmap32::deserialize corrupted data").ToLocalChecked());
      }

      const uint32_t * elems = (const uint32_t *)(bufaschar + 1 + sizeof(uint32_t));
      roaring_bitmap_t * newRoaring = roaring_bitmap_of_ptr(card, elems);
      if (newRoaring == nullptr) {
        return Nan::ThrowError(Nan::New("RoaringBitmap32::deserialize allocation failed").ToLocalChecked());
      }

      ra_clear(&self->roaring.high_low_container);
      self->roaring.high_low_container = std::move(newRoaring->high_low_container);
      return;
    }
    case SERIALIZATION_CONTAINER: {
      roaring_bitmap_t * newRoaring = roaring_bitmap_portable_deserialize_safe(bufaschar + 1, bufLen - 1);
      if (newRoaring == nullptr) {
        return Nan::ThrowError(Nan::New("RoaringBitmap32::deserialize failed").ToLocalChecked());
      }

      ra_clear(&self->roaring.high_low_container);
      self->roaring.high_low_container = std::move(newRoaring->high_low_container);
      return;
    }
  }

  return Nan::ThrowError(Nan::New("RoaringBitmap32::deserialize invalid data").ToLocalChecked());
}