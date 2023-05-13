#ifndef ROARING_NODE_SERIALIZATION_
#define ROARING_NODE_SERIALIZATION_

#include "RoaringBitmap32.h"

#ifndef CROARING_SERIALIZATION_ARRAY_UINT32
constexpr const unsigned char CROARING_SERIALIZATION_ARRAY_UINT32 = 1;
#endif

#ifndef CROARING_SERIALIZATION_CONTAINER
constexpr const unsigned char CROARING_SERIALIZATION_CONTAINER = 2;
#endif

class RoaringBitmapSerializer final {
 public:
  RoaringBitmap32 * self = nullptr;
  SerializationFormat format = SerializationFormat::INVALID;
  v8utils::TypedArrayContent<uint8_t> inputBuffer;

  size_t volatile serializedSize = 0;
  uint8_t * volatile allocatedBuffer = nullptr;

  void parseArguments(const v8::FunctionCallbackInfo<v8::Value> & info) {
    v8::Isolate * isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);
    int formatArgIndex = 0;
    int bufferArgIndex = -1;

    RoaringBitmap32 * bitmap = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
    if (bitmap == nullptr) {
      return v8utils::throwError(isolate, "RoaringBitmap32 serialization on invalid object");
    }
    if (info.Length() <= 0) {
      return v8utils::throwError(isolate, "RoaringBitmap32 serialization format argument was not provided");
    }
    if (info.Length() > 1) {
      if (
        info[0]->IsUint8Array() || info[0]->IsInt8Array() || info[0]->IsUint8ClampedArray() || info[0]->IsArrayBuffer() ||
        info[0]->IsSharedArrayBuffer()) {
        bufferArgIndex = 0;
        formatArgIndex = 1;
      } else if (
        info[1]->IsUint8Array() || info[1]->IsInt8Array() || info[1]->IsUint8ClampedArray() || info[1]->IsArrayBuffer() ||
        info[1]->IsSharedArrayBuffer()) {
        bufferArgIndex = 1;
      } else if (!info[1]->IsUndefined()) {
        return v8utils::throwError(isolate, "RoaringBitmap32 serialization buffer argument was invalid");
      }
    }
    this->format = tryParseSerializationFormat(info[formatArgIndex], isolate);
    if (this->format == SerializationFormat::INVALID) {
      return v8utils::throwError(isolate, "RoaringBitmap32 serialization format argument was invalid");
    }
    if (bufferArgIndex >= 0) {
      if (!this->inputBuffer.set(isolate, info[bufferArgIndex]->ToObject(isolate->GetCurrentContext()))) {
        return v8utils::throwError(isolate, "RoaringBitmap32 serialization buffer argument was invalid");
      }
    }
    this->self = bitmap;
  }

  const char * serialize() {
    size_t buffersize;
    size_t cardinality;
    bool serializeArray = 0;
    switch (this->format) {
      case SerializationFormat::croaring: {
        cardinality = this->self->getSize();
        auto sizeasarray = cardinality * sizeof(uint32_t) + sizeof(uint32_t);
        auto portablesize = roaring_bitmap_portable_size_in_bytes(this->self->roaring);
        if (portablesize < sizeasarray || sizeasarray >= MAX_SERIALIZATION_ARRAY_SIZE_IN_BYTES - 1) {
          buffersize = portablesize + 1;
        } else {
          serializeArray = true;
          buffersize = (size_t)sizeasarray + 1;
        }
        break;
      }

      case SerializationFormat::portable: {
        buffersize = roaring_bitmap_portable_size_in_bytes(this->self->roaring);
        break;
      }

      case SerializationFormat::unsafe_frozen_croaring: {
        buffersize = roaring_bitmap_frozen_size_in_bytes(this->self->roaring);
        break;
      }

      default: return "RoaringBitmap32 serialization format is invalid";
    }

    this->serializedSize = buffersize;
    uint8_t * data = this->inputBuffer.data;

    if (data == nullptr) {
      data =
        (uint8_t *)bare_aligned_malloc(this->format == SerializationFormat::unsafe_frozen_croaring ? 32 : 8, buffersize);
      if (!data) {
        return "RoaringBitmap32 serialization allocation failed";
      }
      this->allocatedBuffer = data;
    } else if (this->inputBuffer.length < buffersize) {
      return "RoaringBitmap32 serialization buffer is too small";
    }

    switch (format) {
      case SerializationFormat::croaring: {
        if (serializeArray) {
          data[0] = CROARING_SERIALIZATION_ARRAY_UINT32;
          memcpy(data + 1, &cardinality, sizeof(uint32_t));
          roaring_bitmap_to_uint32_array(self->roaring, (uint32_t *)(data + 1 + sizeof(uint32_t)));
        } else {
          data[0] = CROARING_SERIALIZATION_CONTAINER;
          roaring_bitmap_portable_serialize(self->roaring, (char *)data + 1);
        }
        break;
      }

      case SerializationFormat::portable: {
        roaring_bitmap_portable_serialize(self->roaring, (char *)data);
        break;
      }

      case SerializationFormat::unsafe_frozen_croaring: {
        roaring_bitmap_frozen_serialize(self->roaring, (char *)data);
        break;
      }

      default: return "RoaringBitmap32 serialization format is invalid";
    }
    return nullptr;
  }

  void done(v8::Isolate * isolate, v8::Local<v8::Value> & result) {
    if (!this->self) {
      return;
    }
    uint8_t * allocatedBuffer = this->allocatedBuffer;

    if (allocatedBuffer) {
      // Create a new buffer using the allocated memory
      v8::MaybeLocal<v8::Object> nodeBufferMaybeLocal =
        node::Buffer::New(isolate, (char *)allocatedBuffer, this->serializedSize, bare_aligned_free_callback, nullptr);
      if (!nodeBufferMaybeLocal.ToLocal(&result)) {
        return v8utils::throwError(isolate, "RoaringBitmap32 serialization failed to create a new buffer");
      }
      this->allocatedBuffer = nullptr;
      return;
    }

    if (!v8utils::v8ValueToBufferWithLimit(
          isolate, self->addonData, this->inputBuffer.bufferPersistent.Get(isolate), this->serializedSize, result)) {
      return v8utils::throwError(isolate, "RoaringBitmap32 serialization failed to create the buffer view");
    }
  }

  ~RoaringBitmapSerializer() { bare_aligned_free(this->allocatedBuffer); }
};

class RoaringBitmapDeserializer final {
 public:
  DeserializationFormat format = DeserializationFormat::INVALID;
  v8::Isolate * isolate = nullptr;

  RoaringBitmap32 * targetBitmap = nullptr;
  v8utils::TypedArrayContent<uint8_t> inputBuffer;

  roaring_bitmap_t_ptr volatile roaring = nullptr;
  uint8_t * volatile frozenBuffer = nullptr;

  ~RoaringBitmapDeserializer() {
    if (this->frozenBuffer != nullptr) {
      bare_aligned_free(this->frozenBuffer);
    }
    if (this->roaring) {
      roaring_bitmap_free(this->roaring);
    }
  }

  const char * setOutput(v8::Isolate * isolate, const v8::MaybeLocal<v8::Value> & valueMaybe, DeserializationFormat format) {
    this->isolate = isolate;
    this->format = format;

    if (valueMaybe.IsEmpty()) {
      return nullptr;
    }

    v8::Local<v8::Value> v;
    if (!valueMaybe.ToLocal(&v) || v->IsNullOrUndefined()) {
      return nullptr;
    }

    if (!this->inputBuffer.set(isolate, v)) {
      return "RoaringBitmap32 deserialization output argument was not a valid typed array";
    }

    return nullptr;
  }

  const char * parseArguments(const v8::FunctionCallbackInfo<v8::Value> & info, bool isInstanceMethod) {
    v8::Isolate * isolate = info.GetIsolate();
    this->isolate = isolate;
    v8::HandleScope scope(isolate);

    if (isInstanceMethod) {
      this->targetBitmap = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
      if (this->targetBitmap == nullptr) {
        return "RoaringBitmap32 deserialization on invalid object";
      }
      if (this->targetBitmap->isFrozen()) {
        return ERROR_FROZEN;
      }
    }

    if (info.Length() < 2) {
      return "RoaringBitmap32 deserialization expects a format and a buffer arguments";
    }

    int bufferArgIndex = 1;
    DeserializationFormat fmt = tryParseDeserializationFormat(info[0], isolate);
    if (fmt == DeserializationFormat::INVALID) {
      bufferArgIndex = 0;
      fmt = tryParseDeserializationFormat(info[1], isolate);
    }
    this->format = fmt;

    if (
      !info[bufferArgIndex]->IsNullOrUndefined() &&
      !this->inputBuffer.set(isolate, info[bufferArgIndex]->ToObject(isolate->GetCurrentContext()))) {
      return "RoaringBitmap32 deserialization buffer argument was invalid";
    }

    return nullptr;
  }

  const char * deserialize() {
    if (this->format == DeserializationFormat::INVALID) {
      return "RoaringBitmap32 deserialization format argument was invalid";
    }

    auto bufLen = this->inputBuffer.length;
    const char * bufaschar = (const char *)this->inputBuffer.data;

    if (bufLen == 0 || !bufaschar) {
      // Empty bitmap for an empty buffer.
      this->roaring = roaring_bitmap_create();
      if (!this->roaring) {
        return "RoaringBitmap32 deserialization failed to create an empty bitmap";
      }
      roaring_bitmap_set_copy_on_write(this->roaring, true);
      return nullptr;
    }

    switch (this->format) {
      case DeserializationFormat::portable: {
        this->roaring = roaring_bitmap_portable_deserialize_safe(bufaschar, bufLen);
        if (!this->roaring) {
          return "RoaringBitmap32::deserialize - portable deserialization failed";
        }
        roaring_bitmap_set_copy_on_write(this->roaring, true);
        return nullptr;
      }

      case DeserializationFormat::croaring: {
        switch ((unsigned char)bufaschar[0]) {
          case CROARING_SERIALIZATION_ARRAY_UINT32: {
            uint32_t card;
            memcpy(&card, bufaschar + 1, sizeof(uint32_t));

            if (card * sizeof(uint32_t) + sizeof(uint32_t) + 1 != bufLen) {
              return "RoaringBitmap32 deserialization corrupted data, wrong cardinality header";
            }

            const uint32_t * elems = (const uint32_t *)(bufaschar + 1 + sizeof(uint32_t));
            this->roaring = roaring_bitmap_of_ptr(card, elems);
            if (!this->roaring) {
              return "RoaringBitmap32 deserialization - uint32 array deserialization failed";
            }
            roaring_bitmap_set_copy_on_write(this->roaring, true);
            return nullptr;
          }

          case CROARING_SERIALIZATION_CONTAINER: {
            this->roaring = roaring_bitmap_portable_deserialize_safe(bufaschar + 1, bufLen - 1);
            if (!this->roaring) {
              return "RoaringBitmap32 deserialization - container deserialization failed";
            }
            roaring_bitmap_set_copy_on_write(this->roaring, true);
            return nullptr;
          }
        }

        return "RoaringBitmap32 deserialization - invalid portable header byte";
      }

      case DeserializationFormat::unsafe_frozen_portable:
      case DeserializationFormat::unsafe_frozen_croaring: {
        this->frozenBuffer = (uint8_t *)bare_aligned_malloc(32, bufLen);
        if (!this->frozenBuffer) {
          return "RoaringBitmap32 deserialization - failed to allocate memory for frozen bitmap";
        }
        memcpy(this->frozenBuffer, bufaschar, bufLen);

        if (format == DeserializationFormat::unsafe_frozen_croaring) {
          this->roaring =
            const_cast<roaring_bitmap_t_ptr>(roaring_bitmap_frozen_view((const char *)this->frozenBuffer, bufLen));
          return this->roaring ? nullptr : "RoaringBitmap32 deserialization - failed to create a frozen view";
        }

        this->roaring =
          const_cast<roaring_bitmap_t_ptr>(roaring_bitmap_portable_deserialize_frozen((const char *)this->frozenBuffer));
        if (!this->roaring) {
          return "RoaringBitmap32 deserialization - failed to create a frozen view";
        }
        roaring_bitmap_set_copy_on_write(this->roaring, true);
        return nullptr;
      }

      default: return "RoaringBitmap32::deserialize - unknown deserialization format";
    }
  }

  void finalizeTargetBitmap(RoaringBitmap32 * targetBitmap) {
    targetBitmap->replaceBitmapInstance(this->isolate, this->roaring);
    this->roaring = nullptr;

    if (this->frozenBuffer) {
      targetBitmap->frozenCounter = RoaringBitmap32::FROZEN_COUNTER_HARD_FROZEN;
      targetBitmap->frozenStorage.data = this->frozenBuffer;
      targetBitmap->frozenStorage.length = std::numeric_limits<size_t>::max();
      this->frozenBuffer = nullptr;
    }
  }
};

#endif  // ROARING_NODE_SERIALIZATION_
