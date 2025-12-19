#ifndef ROARING_NODE_ALIGNED_BUFFERS_
#define ROARING_NODE_ALIGNED_BUFFERS_

#include "v8utils.h"
#include "memory.h"

void _bufferAlignedAlloc(const v8::FunctionCallbackInfo<v8::Value> & info, bool unsafe, bool shared) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);
  v8::Local<v8::Context> context = isolate->GetCurrentContext();

  int64_t size;
  int32_t alignment = 32;
  if (info.Length() < 1 || !info[0]->IsNumber() || !info[0]->IntegerValue(context).To(&size) || size < 0) {
    return v8utils::throwTypeError(isolate, "Buffer size must be a positive integer");
  }

  if (info.Length() >= 2 && !info[1]->IsUndefined()) {
    if (!info[1]->IsNumber() || !info[1]->Int32Value(context).To(&alignment) || alignment <= 0) {
      return v8utils::throwTypeError(isolate, "Buffer alignment must be a positive integer");
    }
  }

  if (size < 0) {
    size = 0;
  }

  if ((uint64_t)size > node::Buffer::kMaxLength || (uint64_t)size + alignment >= node::Buffer::kMaxLength) {
    return v8utils::throwTypeError(isolate, "Buffer size is too large");
  }

  if (alignment > 1024) {
    return v8utils::throwTypeError(isolate, "Buffer alignment is too large");
  }

  void * ptr = bare_aligned_malloc(alignment, size);
  if (!ptr) {
    return v8utils::throwError(isolate, "Buffer memory allocation failed");
  }

  if (!unsafe) {
    memset(ptr, 0, size);
  }

  if (shared) {
    AddonData * addonData = AddonData::get(info);
    if (addonData == nullptr) {
      bare_aligned_free(ptr);
      return v8utils::throwError(isolate, "Invalid call");
    }

    auto backingStore = v8::SharedArrayBuffer::NewBackingStore(ptr, (size_t)size, bare_aligned_free_callback2, nullptr);
    if (!backingStore) {
      bare_aligned_free(ptr);
      return v8utils::throwError(isolate, "Buffer creation failed");
    }
    auto sharedBuf = v8::SharedArrayBuffer::New(isolate, std::move(backingStore));
    v8::Local<v8::Value> bufferObj;
    if (sharedBuf.IsEmpty() || !v8utils::bufferFromArrayBuffer(isolate, addonData, sharedBuf, 0, size, bufferObj)) {
      bare_aligned_free(ptr);
      return v8utils::throwError(isolate, "Buffer creation failed");
    }
    info.GetReturnValue().Set(bufferObj);
    return;
  }

  v8::MaybeLocal<v8::Object> bufferMaybe;
  bufferMaybe = node::Buffer::New(isolate, (char *)ptr, size, bare_aligned_free_callback, nullptr);
  v8::Local<v8::Value> bufferValue;
  if (!bufferMaybe.ToLocal(&bufferValue) || bufferValue.IsEmpty()) {
    bare_aligned_free(ptr);
    return v8utils::throwError(isolate, "Buffer creation failed");
  }

  info.GetReturnValue().Set(bufferValue);
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
  v8::Local<v8::Context> context = isolate->GetCurrentContext();

  if (info.Length() < 1) {
    return info.GetReturnValue().Set(false);
  }

  // Second parameter must be a positive integer
  int32_t alignment = 32;
  if (info.Length() >= 2 && !info[1]->IsUndefined()) {
    if (!info[1]->IsNumber() || !info[1]->Int32Value(context).To(&alignment) || alignment < 0) {
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
