#ifndef ROARING_NODE_ROARINGBITMAP32_STATIC_OPS_
#define ROARING_NODE_ROARINGBITMAP32_STATIC_OPS_

#include "RoaringBitmap32.h"

void RoaringBitmap32::andInPlace(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (!self) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  if (self->isFrozen()) {
    return v8utils::throwError(isolate, ERROR_FROZEN);
  }
  if (info.Length() > 0) {
    auto const & arg = info[0];
    RoaringBitmap32 * other =
      ObjectWrap::TryUnwrap<RoaringBitmap32>(arg, globalAddonData.RoaringBitmap32_constructorTemplate, isolate);
    if (other != nullptr) {
      roaring_bitmap_and_inplace(self->roaring, other->roaring);
      self->invalidate();
      return info.GetReturnValue().Set(info.Holder());
    } else {
      RoaringBitmap32 tmp(&globalAddonData, 0U);
      if (roaringAddMany(isolate, &tmp, arg)) {
        roaring_bitmap_and_inplace(self->roaring, tmp.roaring);
        self->invalidate();
        return info.GetReturnValue().Set(info.Holder());
      }
    }
  }

  return v8utils::throwTypeError(isolate, "Uint32Array, RoaringBitmap32 or Iterable<number> expected");
}

void RoaringBitmap32::xorInPlace(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (!self) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  if (self->isFrozen()) {
    return v8utils::throwError(isolate, ERROR_FROZEN);
  }
  if (info.Length() > 0) {
    auto const & arg = info[0];
    RoaringBitmap32 * other =
      ObjectWrap::TryUnwrap<RoaringBitmap32>(arg, globalAddonData.RoaringBitmap32_constructorTemplate, isolate);
    if (other != nullptr) {
      roaring_bitmap_xor_inplace(self->roaring, other->roaring);
      self->invalidate();
      return info.GetReturnValue().Set(info.Holder());
    } else {
      RoaringBitmap32 tmp(&globalAddonData, 0U);
      roaringAddMany(info.GetIsolate(), &tmp, arg);
      roaring_bitmap_xor_inplace(self->roaring, tmp.roaring);
      self->invalidate();
      return info.GetReturnValue().Set(info.Holder());
    }
  }

  return v8utils::throwTypeError(isolate, "Uint32Array, RoaringBitmap32 or Iterable<number> expected");
}

#endif  // ROARING_NODE_ROARINGBITMAP32_STATIC_OPS_
