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

bool argumentIsValidUint32ArrayOutput(const v8::MaybeLocal<v8::Value> & value) {
  if (value.IsEmpty()) {
    return false;
  }
  v8::Local<v8::Value> local;
  return value.ToLocal(&local) && argumentIsValidUint32ArrayOutput(local);
}

namespace v8utils {

  template <int N>
  inline void throwError(v8::Isolate * isolate, const char (&message)[N]) {
    isolate->ThrowException(
      v8::Exception::Error(v8::String::NewFromUtf8Literal(isolate, message, v8::NewStringType::kInternalized)));
  }

  template <int N>
  inline void throwTypeError(v8::Isolate * isolate, const char (&message)[N]) {
    isolate->ThrowException(
      v8::Exception::TypeError(v8::String::NewFromUtf8Literal(isolate, message, v8::NewStringType::kInternalized)));
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
      v8::Exception::Error(v8::String::NewFromUtf8Literal(isolate, "Operation failed", v8::NewStringType::kInternalized)));
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
    isolate->ThrowException(v8::Exception::TypeError(
      v8::String::NewFromUtf8Literal(isolate, "Operation failed", v8::NewStringType::kInternalized)));
  }

  void throwTypeError(v8::Isolate * isolate, const char * context, const char * message) {
    v8::HandleScope scope(isolate);
    auto a = v8::String::NewFromUtf8(isolate, context, v8::NewStringType::kInternalized);
    auto b = v8::String::NewFromUtf8(isolate, message, v8::NewStringType::kInternalized);
    auto msg = a.IsEmpty() ? b : b.IsEmpty() ? a : v8::String::Concat(isolate, a.ToLocalChecked(), b.ToLocalChecked());
    isolate->ThrowException(v8::Exception::TypeError(msg.IsEmpty() ? v8::String::Empty(isolate) : msg.ToLocalChecked()));
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
    std::shared_ptr<v8::BackingStore> backingStore;
    T * data;

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
      this->backingStore.reset();
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
          v8::Local<v8::ArrayBufferView> array = v8::Local<v8::ArrayBufferView>::Cast(from);
          auto arrayBuffer = array->Buffer();
          if (arrayBuffer.IsEmpty()) {
            this->reset();
            return false;
          }
          this->backingStore = arrayBuffer->GetBackingStore();
          auto data = this->backingStore->Data();
          if (data) {
            this->data = (T *)((uint8_t *)(data) + array->ByteOffset());
            this->length = array->ByteLength() / sizeof(T);
          } else {
            this->data = nullptr;
            this->length = 0;
          }
          return true;
        }

        if (from->IsArrayBuffer()) {
          v8::Local<v8::ArrayBuffer> arrayBuffer = v8::Local<v8::ArrayBuffer>::Cast(from);
          this->backingStore = arrayBuffer->GetBackingStore();
          auto data = this->backingStore->Data();
          if (data) {
            this->data = (T *)(data);
            this->length = arrayBuffer->ByteLength() / sizeof(T);
          } else {
            this->data = nullptr;
            this->length = 0;
          }
          return true;
        }

        if (from->IsSharedArrayBuffer()) {
          v8::Local<v8::SharedArrayBuffer> arrayBuffer = v8::Local<v8::SharedArrayBuffer>::Cast(from);
          this->backingStore = arrayBuffer->GetBackingStore();
          auto data = this->backingStore->Data();
          if (data) {
            this->data = (T *)(data);
            this->length = arrayBuffer->ByteLength() / sizeof(T);
          } else {
            this->data = nullptr;
            this->length = 0;
          }
          return true;
        }
      }

      this->reset();
      return false;
    }
  };

}  // namespace v8utils

#endif  // ROARING_NODE_V8UTILS_
