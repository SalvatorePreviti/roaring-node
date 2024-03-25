#ifndef ROARING_NODE_ALIGNED_BUFFERS_
#define ROARING_NODE_ALIGNED_BUFFERS_

#include "v8utils.h"
#include "memory.h"

void _bufferAlignedAlloc(const v8::FunctionCallbackInfo<v8::Value> & info, bool unsafe, bool shared) {
  v8::Isolate * isolate = info.GetIsolate();

  int64_t size;
  int32_t alignment = 32;
  if (
    info.Length() < 1 || !info[0]->IsNumber() || !info[0]->IntegerValue(isolate->GetCurrentContext()).To(&size) ||
    size < 0) {
    return v8utils::throwTypeError(isolate, "Buffer size must be a positive integer");
  }

  if (info.Length() >= 2 && !info[1]->IsUndefined()) {
    if (!info[1]->IsNumber() || !info[1]->Int32Value(isolate->GetCurrentContext()).To(&alignment) || alignment <= 0) {
      return v8utils::throwTypeError(isolate, "Buffer alignment must be a positive integer");
    }
  }

  if (size < 0) {
    size = 0;
  }

  if (alignment > 1024) {
    return v8utils::throwTypeError(isolate, "Buffer alignment is too large");
  }

  if ((uint64_t)size > node::Buffer::kMaxLength || (uint64_t)size + alignment >= node::Buffer::kMaxLength) {
    return v8utils::throwTypeError(isolate, "Buffer size is too large");
  }

  void * ptr = bare_aligned_malloc(alignment, size);
  if (ptr == nullptr) {
    return v8utils::throwError(isolate, "Buffer memory allocation failed");
  }

  if (!unsafe) {
    memset(ptr, 0, size);
  }

  v8::Local<v8::Value> bufferValue;
  if (shared) {
    auto sharedBuf = v8::SharedArrayBuffer::New(
      isolate, v8::SharedArrayBuffer::NewBackingStore(ptr, (size_t)size, bare_aligned_free_callback2, nullptr));
    if (sharedBuf.IsEmpty()) {
      return v8utils::throwError(isolate, "Buffer creation failed");
    }
    auto result = v8::Uint8Array::New(sharedBuf, 0, size);
    if (result.IsEmpty()) {
      return v8utils::throwError(isolate, "Buffer creation failed");
    }
    auto bufferMaybe = node::Buffer::New(isolate, result->Buffer(), 0, size);
    if (!bufferMaybe.ToLocal(&bufferValue) || bufferValue.IsEmpty()) {
      return v8utils::throwError(isolate, "Buffer creation failed");
    }
    info.GetReturnValue().Set(bufferValue);
  } else {
    auto bufferMaybe = node::Buffer::New(isolate, (char *)ptr, size, bare_aligned_free_callback, nullptr);
    if (!bufferMaybe.ToLocal(&bufferValue) || bufferValue.IsEmpty()) {
      bare_aligned_free(ptr);
      return v8utils::throwError(isolate, "Buffer creation failed");
    }
    info.GetReturnValue().Set(bufferValue);
  }
}

void bufferAlignedAlloc(const v8::FunctionCallbackInfo<v8::Value> & info) { _bufferAlignedAlloc(info, false, false); }

void bufferAlignedAllocUnsafe(const v8::FunctionCallbackInfo<v8::Value> & info) { _bufferAlignedAlloc(info, true, false); }

void bufferAlignedAllocShared(const v8::FunctionCallbackInfo<v8::Value> & info) { _bufferAlignedAlloc(info, false, true); }

void bufferAlignedAllocSharedUnsafe(const v8::FunctionCallbackInfo<v8::Value> & info) {
  _bufferAlignedAlloc(info, true, true);
}

void isBufferAligned(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  if (info.Length() < 1) {
    return info.GetReturnValue().Set(false);
  }

  // Second parameter must be a positive integer
  int32_t alignment = 32;
  if (info.Length() >= 2 && !info[1]->IsUndefined()) {
    if (!info[1]->IsNumber() || !info[1]->Int32Value(isolate->GetCurrentContext()).To(&alignment) || alignment < 0) {
      return info.GetReturnValue().Set(false);
    }
  }

  if (alignment == 0) {
    return info.GetReturnValue().Set(true);
  }

  v8utils::TypedArrayContent<uint8_t> content(isolate, info[0]);
  info.GetReturnValue().Set(is_pointer_aligned(content.data, alignment));
}

void AlignedBuffers_Init(v8::Local<v8::Object> exports, AddonData * addonData) {
  addonData->setMethod(exports, "bufferAlignedAlloc", bufferAlignedAlloc);
  addonData->setMethod(exports, "bufferAlignedAllocUnsafe", bufferAlignedAllocUnsafe);
  addonData->setMethod(exports, "bufferAlignedAllocShared", bufferAlignedAllocShared);
  addonData->setMethod(exports, "bufferAlignedAllocSharedUnsafe", bufferAlignedAllocSharedUnsafe);
  addonData->setMethod(exports, "isBufferAligned", isBufferAligned);
}

#endif  // ROARING_NODE_ALIGNED_BUFFERS_
