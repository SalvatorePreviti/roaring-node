#ifndef ROARING_NODE_V8UTILS_
#define ROARING_NODE_V8UTILS_

#include "includes.h"
#include "addon-data.h"
#include "addon-strings.h"
#include "memory.h"
#include "object-wrap.h"

bool argumentIsValidUint32ArrayOutput(const v8::Local<v8::Value> & value) {
  return !value.IsEmpty() &&
    (value->IsUint32Array() || value->IsInt32Array() || value->IsArrayBuffer() || value->IsSharedArrayBuffer());
}

namespace v8utils {

  inline bool v8ValueToUint32Fast(v8::Local<v8::Context> context, v8::Local<v8::Value> value, uint32_t & result) {
    if (value.IsEmpty()) {
      return false;
    }
    if (value->IsUint32()) {
      result = value.As<v8::Uint32>()->Value();
      return true;
    }
    if (value->IsInt32()) {
      const int32_t n = value.As<v8::Int32>()->Value();
      if (n < 0) {
        return false;
      }
      result = static_cast<uint32_t>(n);
      return true;
    }
    if (value->IsNullOrUndefined()) {
      return false;
    }
    double d;
    if (value->IsNumber()) {
      d = value.As<v8::Number>()->Value();
    } else if (!value->NumberValue(context).To(&d)) {
      return false;
    }
    if (std::isnan(d)) {
      return false;
    }
    const int64_t n = static_cast<int64_t>(d);
    if (n < 0 || n > static_cast<int64_t>(UINT32_MAX)) {
      return false;
    }
    result = static_cast<uint32_t>(n);
    return true;
  }

  template <int N>
  inline void throwError(v8::Isolate * isolate, const char (&message)[N]) {
    isolate->ThrowException(v8::Exception::Error(NEW_LITERAL_V8_STRING(isolate, message, v8::NewStringType::kInternalized)));
  }

  template <int N>
  inline void throwTypeError(v8::Isolate * isolate, const char (&message)[N]) {
    isolate->ThrowException(
      v8::Exception::TypeError(NEW_LITERAL_V8_STRING(isolate, message, v8::NewStringType::kInternalized)));
  }

  void throwError(v8::Isolate * isolate, const char * message) {
    v8::HandleScope scope(isolate);
    if (message != nullptr && message[0] != '\0') {
      auto msg = v8::String::NewFromUtf8(isolate, message, v8::NewStringType::kInternalized);
      v8::Local<v8::String> msgLocal;
      if (msg.ToLocal(&msgLocal)) {
        isolate->ThrowException(v8::Exception::Error(msgLocal));
        return;
      }
    }
    isolate->ThrowException(
      v8::Exception::Error(NEW_LITERAL_V8_STRING(isolate, "Operation failed", v8::NewStringType::kInternalized)));
  }

  void throwTypeError(v8::Isolate * isolate, const char * message) {
    v8::HandleScope scope(isolate);
    if (message != nullptr && message[0] != '\0') {
      auto msg = v8::String::NewFromUtf8(isolate, message, v8::NewStringType::kInternalized);
      v8::Local<v8::String> msgLocal;
      if (msg.ToLocal(&msgLocal)) {
        isolate->ThrowException(v8::Exception::TypeError(msgLocal));
        return;
      }
    }
    isolate->ThrowException(
      v8::Exception::TypeError(NEW_LITERAL_V8_STRING(isolate, "Operation failed", v8::NewStringType::kInternalized)));
  }

  void throwTypeError(v8::Isolate * isolate, const char * context, const char * message) {
    v8::HandleScope scope(isolate);
    auto a = v8::String::NewFromUtf8(isolate, context, v8::NewStringType::kInternalized);
    auto b = v8::String::NewFromUtf8(isolate, message, v8::NewStringType::kInternalized);
    auto msg = a.IsEmpty() ? b : b.IsEmpty() ? a : v8::String::Concat(isolate, a.ToLocalChecked(), b.ToLocalChecked());
    isolate->ThrowException(v8::Exception::TypeError(msg.IsEmpty() ? v8::String::Empty(isolate) : msg.ToLocalChecked()));
  }

  template <int N>
  void defineHiddenField(
    v8::Isolate * isolate, v8::Local<v8::Object> target, const char (&literal)[N], v8::Local<v8::Value> value) {
    v8::HandleScope scope(isolate);
    v8::PropertyDescriptor propertyDescriptor(value, false);
    propertyDescriptor.set_configurable(false);
    propertyDescriptor.set_enumerable(false);
    auto name = NEW_LITERAL_V8_STRING(isolate, literal, v8::NewStringType::kInternalized);
    ignoreMaybeResult(target->DefineProperty(isolate->GetCurrentContext(), name, propertyDescriptor));
  }

  void defineHiddenField(
    v8::Isolate * isolate, v8::Local<v8::Object> target, v8::Local<v8::Name> name, v8::Local<v8::Value> value) {
    v8::HandleScope scope(isolate);
    v8::PropertyDescriptor propertyDescriptor(value, false);
    propertyDescriptor.set_configurable(false);
    propertyDescriptor.set_enumerable(false);
    ignoreMaybeResult(target->DefineProperty(isolate->GetCurrentContext(), name, propertyDescriptor));
  }

  template <int N>
  void defineReadonlyField(
    v8::Isolate * isolate, v8::Local<v8::Object> target, const char (&literal)[N], v8::Local<v8::Value> value) {
    v8::HandleScope scope(isolate);
    v8::PropertyDescriptor propertyDescriptor(value, false);
    propertyDescriptor.set_configurable(false);
    propertyDescriptor.set_enumerable(true);

    auto name = NEW_LITERAL_V8_STRING(isolate, literal, v8::NewStringType::kInternalized);
    ignoreMaybeResult(target->DefineProperty(isolate->GetCurrentContext(), name, propertyDescriptor));
  }

  bool bufferFromArrayBuffer(
    v8::Isolate * isolate,
    AddonData * addonData,
    v8::Local<v8::Value> buffer,
    size_t offset,
    size_t length,
    v8::Local<v8::Value> & result) {
    if (buffer.IsEmpty()) {
      return false;
    }

    if (buffer->IsSharedArrayBuffer()) {
      auto buf = buffer.As<v8::SharedArrayBuffer>();
      if (buf.IsEmpty()) {
        return false;
      }
      auto uint8Array = v8::Uint8Array::New(buf, offset, length);
      if (uint8Array.IsEmpty()) {
        return false;
      }
      return node::Buffer::New(isolate, uint8Array->Buffer(), offset, length).ToLocal(&result);
    } else {
      auto buf = buffer.As<v8::ArrayBuffer>();
      if (buf.IsEmpty()) {
        return false;
      }
      return node::Buffer::New(isolate, buf, offset, length).ToLocal(&result);
    }
  }

  bool v8ValueToBufferWithLimit(
    v8::Isolate * isolate,
    AddonData * addonData,
    v8::MaybeLocal<v8::Value> value,
    size_t length,
    v8::Local<v8::Value> & result) {
    v8::Local<v8::Value> localValue;
    if (value.ToLocal(&localValue) && !localValue.IsEmpty()) {
      if (localValue->IsUint8Array()) {
        v8::Local<v8::Uint8Array> array = localValue.As<v8::Uint8Array>();
        if (!array.IsEmpty()) {
          if (node::Buffer::HasInstance(localValue) && node::Buffer::Length(localValue) == length) {
            result = array;
            return true;
          }
          if (array->ByteLength() >= length) {
            return bufferFromArrayBuffer(isolate, addonData, array->Buffer(), array->ByteOffset(), length, result);
          }
        }
        return false;
      }
      if (localValue->IsTypedArray()) {
        auto array = localValue.As<v8::TypedArray>();
        if (!array.IsEmpty() && array->ByteLength() >= length) {
          return bufferFromArrayBuffer(isolate, addonData, array->Buffer(), array->ByteOffset(), length, result);
        }
        return false;
      }
      if (localValue->IsArrayBufferView()) {
        auto array = localValue.As<v8::ArrayBufferView>();
        if (!array.IsEmpty() && array->ByteLength() >= length) {
          return bufferFromArrayBuffer(isolate, addonData, array->Buffer(), array->ByteOffset(), length, result);
        }
        return false;
      }
      if (localValue->IsArrayBuffer()) {
        auto array = localValue.As<v8::ArrayBuffer>();
        if (!array.IsEmpty() && array->ByteLength() >= length) {
          return bufferFromArrayBuffer(isolate, addonData, array, 0, length, result);
        }
        return false;
      }
      if (localValue->IsSharedArrayBuffer()) {
        auto array = localValue.As<v8::SharedArrayBuffer>();
        if (!array.IsEmpty() && array->ByteLength() >= length) {
          return bufferFromArrayBuffer(isolate, addonData, array, 0, length, result);
        }
        return false;
      }
    }
    return false;
  }

  bool v8ValueToUint32ArrayWithLimit(
    v8::Isolate * isolate, v8::MaybeLocal<v8::Value> value, size_t length, v8::Local<v8::Value> & result) {
    v8::Local<v8::Value> localValue;
    if (!value.ToLocal(&localValue)) {
      return {};
    }
    if (localValue->IsUint32Array()) {
      auto array = localValue.As<v8::Uint32Array>();
      if (!array.IsEmpty()) {
        auto arrayLength = array->Length();
        if (arrayLength == length) {
          result = array;
          return true;
        }
        if (arrayLength > length) {
          result = v8::Uint32Array::New(array->Buffer(), array->ByteOffset(), length);
          return !result.IsEmpty();
        }
      }
      return false;
    }
    if (localValue->IsTypedArray()) {
      auto array = localValue.As<v8::TypedArray>();
      if (array->ByteLength() >= length * sizeof(uint32_t)) {
        result = v8::Uint32Array::New(array->Buffer(), array->ByteOffset(), length);
        return !result.IsEmpty();
      }
      return false;
    }
    if (localValue->IsArrayBufferView()) {
      auto array = localValue.As<v8::ArrayBufferView>();
      if (array->ByteLength() >= length * sizeof(uint32_t)) {
        result = v8::Uint32Array::New(array->Buffer(), array->ByteOffset(), length);
        return !result.IsEmpty();
      }
      return false;
    }
    if (localValue->IsArrayBuffer()) {
      auto array = localValue.As<v8::ArrayBuffer>();
      if (array->ByteLength() >= length * sizeof(uint32_t)) {
        result = v8::Uint32Array::New(array, 0, length);
        return !result.IsEmpty();
      }
      return false;
    }
    if (localValue->IsSharedArrayBuffer()) {
      auto array = localValue.As<v8::SharedArrayBuffer>();
      if (array->ByteLength() >= length * sizeof(uint32_t)) {
        result = v8::Uint32Array::New(array, 0, length);
        return !result.IsEmpty();
      }
      return false;
    }
    return false;
  }

  template <typename T>
  class TypedArrayContent final {
   public:
    size_t length;
    T * data;
    v8::Global<v8::Value> bufferPersistent;
    std::shared_ptr<v8::BackingStore> backingStore;

    inline TypedArrayContent() : length(0), data(nullptr) {}

    template <typename Q>
    inline explicit TypedArrayContent(v8::Isolate * isolate, v8::Local<Q> from) {
      set(isolate, from);
    }

    template <typename Q>
    inline explicit TypedArrayContent(v8::Isolate * isolate, v8::MaybeLocal<Q> from) {
      set(isolate, from);
    }

    inline void reset() {
      this->length = 0;
      this->data = nullptr;
      this->backingStore = nullptr;
      this->bufferPersistent.Reset();
    }

    template <typename Q>
    bool set(v8::Isolate * isolate, const v8::MaybeLocal<Q> & from) {
      v8::Local<Q> local;
      if (from.ToLocal(&local)) {
        return this->set(isolate, local);
      }
      this->reset();
      return false;
    }

    template <typename Q>
    bool set(v8::Isolate * isolate, const v8::Local<Q> & from) {
      if (!from.IsEmpty()) {
        if (from->IsArrayBufferView()) {
          bufferPersistent.Reset(isolate, from);
          v8::Local<v8::ArrayBufferView> array = v8::Local<v8::ArrayBufferView>::Cast(from);
          this->length = array->ByteLength() / sizeof(T);
          auto arrayBuffer = array->Buffer();
          this->backingStore = arrayBuffer->GetBackingStore();
          this->data = (T *)((uint8_t *)(this->backingStore->Data()) + array->ByteOffset());
          return true;
        }

        if (from->IsArrayBuffer()) {
          bufferPersistent.Reset(isolate, from);
          v8::Local<v8::ArrayBuffer> arrayBuffer = v8::Local<v8::ArrayBuffer>::Cast(from);
          this->length = arrayBuffer->ByteLength() / sizeof(T);
          this->backingStore = arrayBuffer->GetBackingStore();
          this->data = (T *)((uint8_t *)(this->backingStore->Data()));
          return true;
        }

        if (from->IsSharedArrayBuffer()) {
          bufferPersistent.Reset(isolate, from);
          v8::Local<v8::SharedArrayBuffer> arrayBuffer = v8::Local<v8::SharedArrayBuffer>::Cast(from);
          this->length = arrayBuffer->ByteLength() / sizeof(T);
          this->backingStore = arrayBuffer->GetBackingStore();
          this->data = (T *)((uint8_t *)(this->backingStore->Data()));
          return true;
        }
      }

      this->reset();
      return false;
    }
  };

}  // namespace v8utils

#endif  // ROARING_NODE_V8UTILS_
