#ifndef ROARING_NODE_ALIGNED_BUFFERS_
#define ROARING_NODE_ALIGNED_BUFFERS_

#include "memory.h"
#include "v8utils.h"

inline void _bufAlignedAlloc(const v8::FunctionCallbackInfo<v8::Value> & info, bool unsafe, bool shared) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

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

  v8::Local<v8::Value> bufferValue;
  if (shared) {
    // Old implementation: doesn't seems to work on ubuntu, and I don't know why.
    // It seems that it causes random segfaults when a worker thread terminates, evident using vitest
    //
    //     std::shared_ptr<v8::BackingStore> backing_store =
    //       v8::SharedArrayBuffer::NewBackingStore(ptr, (size_t)size, bare_aligned_free_callback2, ptr);
    //
    //     auto sharedBuf = v8::SharedArrayBuffer::New(isolate, backing_store);
    //     if (sharedBuf.IsEmpty()) {
    //       return v8utils::throwError(isolate, "Buffer creation failed");
    //     }

    auto sharedBuf = v8::SharedArrayBuffer::New(isolate, size + alignment);
    if (sharedBuf.IsEmpty()) {
      return v8utils::throwError(isolate, "Buffer creation failed");
    }

    std::shared_ptr<v8::BackingStore> backingStore = sharedBuf->GetBackingStore();
    if (!backingStore) {
      return v8utils::throwError(isolate, "Buffer creation failed");
    }

    void * ptr = backingStore->Data();
    if (ptr == nullptr) {
      return v8utils::throwError(isolate, "Buffer memory allocation failed");
    }

    size_t byteOffset = alignment > 0 ? alignment - ((size_t)ptr % alignment) : 0;

    auto uint8Array = v8::Uint8Array::New(sharedBuf, 0, size);
    if (uint8Array.IsEmpty()) {
      return v8utils::throwError(isolate, "Buffer creation failed");
    }

    auto bufferMaybe = node::Buffer::New(isolate, uint8Array->Buffer(), byteOffset, size);
    if (!bufferMaybe.ToLocal(&bufferValue) || bufferValue.IsEmpty()) {
      return v8utils::throwError(isolate, "Buffer creation failed");
    }

  } else {
    void * ptr = bare_aligned_malloc(alignment, size);
    if (ptr == nullptr) {
      return v8utils::throwError(isolate, "Buffer memory allocation failed");
    }

    if (!unsafe) {
      memset(ptr, 0, size);
    }
    auto bufferMaybe = node::Buffer::New(isolate, (char *)ptr, size, bare_aligned_free_callback, ptr);
    if (!bufferMaybe.ToLocal(&bufferValue) || bufferValue.IsEmpty()) {
      bare_aligned_free(ptr);
      return v8utils::throwError(isolate, "Buffer creation failed");
    }
  }

  info.GetReturnValue().Set(bufferValue);
}

void bufferAlignedAlloc(const v8::FunctionCallbackInfo<v8::Value> & info) { _bufAlignedAlloc(info, false, false); }

void bufferAlignedAllocUnsafe(const v8::FunctionCallbackInfo<v8::Value> & info) { _bufAlignedAlloc(info, true, false); }

void bufferAlignedAllocShared(const v8::FunctionCallbackInfo<v8::Value> & info) { _bufAlignedAlloc(info, false, true); }

void bufferAlignedAllocSharedUnsafe(const v8::FunctionCallbackInfo<v8::Value> & info) { _bufAlignedAlloc(info, true, true); }

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
  NODE_SET_METHOD(exports, "bufferAlignedAlloc", bufferAlignedAlloc);
  NODE_SET_METHOD(exports, "bufferAlignedAllocUnsafe", bufferAlignedAllocUnsafe);
  NODE_SET_METHOD(exports, "bufferAlignedAllocShared", bufferAlignedAllocShared);
  NODE_SET_METHOD(exports, "bufferAlignedAllocSharedUnsafe", bufferAlignedAllocSharedUnsafe);
  NODE_SET_METHOD(exports, "isBufferAligned", isBufferAligned);
}

#endif  // ROARING_NODE_ALIGNED_BUFFERS_
