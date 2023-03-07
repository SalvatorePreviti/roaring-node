#ifndef ROARING_NODE_ALIGNED_BUFFERS_
#define ROARING_NODE_ALIGNED_BUFFERS_

#include "v8utils.h"
#include "memory.h"

static void _bufferAlignedAlloc(const v8::FunctionCallbackInfo<v8::Value> & info, bool unsafe) {
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

  v8::MaybeLocal<v8::Object> bufferMaybe =
    node::Buffer::New(isolate, (char *)ptr, size, bare_aligned_free_callback, nullptr);

  v8::Local<v8::Object> bufferObj;
  if (!bufferMaybe.ToLocal(&bufferObj)) {
    bare_aligned_free(ptr);
    return v8utils::throwError(isolate, "Buffer creation failed");
  }

  info.GetReturnValue().Set(bufferObj);
}

static void bufferAlignedAlloc(const v8::FunctionCallbackInfo<v8::Value> & info) { _bufferAlignedAlloc(info, false); }

static void bufferAlignedAllocUnsafe(const v8::FunctionCallbackInfo<v8::Value> & info) { _bufferAlignedAlloc(info, true); }

static void isBufferAligned(const v8::FunctionCallbackInfo<v8::Value> & info) {
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

static void AlignedBuffers_Init(v8::Local<v8::Object> exports) {
  NODE_SET_METHOD(exports, "bufferAlignedAlloc", bufferAlignedAlloc);
  NODE_SET_METHOD(exports, "bufferAlignedAllocUnsafe", bufferAlignedAllocUnsafe);
  NODE_SET_METHOD(exports, "isBufferAligned", isBufferAligned);
}

#endif  // ROARING_NODE_ALIGNED_BUFFERS_
