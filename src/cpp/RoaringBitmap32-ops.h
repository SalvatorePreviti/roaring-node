#ifndef ROARING_NODE_ROARINGBITMAP32_OPS_
#define ROARING_NODE_ROARINGBITMAP32_OPS_

#include "RoaringBitmap32.h"

inline bool roaringAddMany(v8::Isolate * isolate, RoaringBitmap32 * self, v8::Local<v8::Value> arg, bool replace = false) {
  if (self->isFrozen()) {
    v8utils::throwError(isolate, ERROR_FROZEN);
    return false;
  }

  if (arg.IsEmpty()) {
    return false;
  }

  if (arg->IsNullOrUndefined()) {
    if (replace && self->roaring->high_low_container.containers != nullptr) {
      roaring_bitmap_clear(self->roaring);
    }
    return true;
  }

  if (!arg->IsObject()) {
    return false;
  }

  if (arg->IsUint32Array() || arg->IsInt32Array() || arg->IsArrayBuffer() || arg->IsSharedArrayBuffer()) {
    if (replace && self->roaring->high_low_container.containers != nullptr) {
      roaring_bitmap_clear(self->roaring);
    }
    const v8utils::TypedArrayContent<uint32_t> typedArray(isolate, arg);
    roaring_bitmap_add_many(self->roaring, typedArray.length, typedArray.data);
    self->invalidate();
    return true;
  }

  RoaringBitmap32 * other = ObjectWrap::TryUnwrap<RoaringBitmap32>(arg, isolate);
  if (other != nullptr) {
    if (self != other) {
      if (replace || self->roaring->high_low_container.containers == nullptr) {
        roaring_bitmap_overwrite(self->roaring, other->roaring);
      } else {
        roaring_bitmap_or_inplace(self->roaring, other->roaring);
      }
      self->invalidate();
    }
    return true;
  }

  AddonData * addonData = self->addonData;

  v8::Local<v8::Value> argv[] = {arg};
  auto tMaybe = addonData->Uint32Array_from.Get(isolate)->Call(
    isolate->GetCurrentContext(), addonData->Uint32Array.Get(isolate), 1, argv);
  v8::Local<v8::Value> t;
  if (!tMaybe.ToLocal(&t)) return false;

  const v8utils::TypedArrayContent<uint32_t> typedArray(isolate, t);
  if (replace) {
    roaring_bitmap_clear(self->roaring);
  }
  roaring_bitmap_add_many(self->roaring, typedArray.length, typedArray.data);
  self->invalidate();
  return true;
}

void RoaringBitmap32_andCardinality(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  RoaringBitmap32 * other = ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0);
  info.GetReturnValue().Set(self && other ? (double)roaring_bitmap_and_cardinality(self->roaring, other->roaring) : -1);
}

void RoaringBitmap32_orCardinality(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  RoaringBitmap32 * other = ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0);
  info.GetReturnValue().Set(self && other ? (double)roaring_bitmap_or_cardinality(self->roaring, other->roaring) : -1);
}

void RoaringBitmap32_andNotCardinality(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  RoaringBitmap32 * other = ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0);
  info.GetReturnValue().Set(self && other ? (double)roaring_bitmap_andnot_cardinality(self->roaring, other->roaring) : -1);
}

void RoaringBitmap32_xorCardinality(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  if (self == nullptr) {
    return info.GetReturnValue().Set(0);
  }
  RoaringBitmap32 * other = ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0);
  info.GetReturnValue().Set(self && other ? (double)roaring_bitmap_xor_cardinality(self->roaring, other->roaring) : -1);
}

void RoaringBitmap32_add(const v8::FunctionCallbackInfo<v8::Value> & info) {
  auto isolate = info.GetIsolate();

  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (!self) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  if (self->isFrozen()) {
    return v8utils::throwError(isolate, ERROR_FROZEN);
  }

  info.GetReturnValue().Set(info.Holder());

  bool changed = false;
  auto roaring = self->roaring;
  int len = info.Length();

  uint32_t v = 0;
  for (int i = 0; i < len; ++i) {
    auto arg = info[i];
    if (arg->IsInt32()) {
      int32_t n = arg->Int32Value(isolate->GetCurrentContext()).FromJust();
      if (n < 0) {
        continue;
      }
      v = (uint32_t)n;
    } else if (arg->IsUint32()) {
      v = arg->Uint32Value(isolate->GetCurrentContext()).FromJust();
    } else {
      double d;
      if (arg->IsNull() || arg->IsUndefined()) {
        continue;
      }
      if (!arg->NumberValue(isolate->GetCurrentContext()).To(&d) || std::isnan(d)) {
        continue;
      }
      int64_t n = (int64_t)d;
      if (n < 0 || n > UINT32_MAX) {
        continue;
      }
      v = (uint32_t)n;
    }
    if (roaring_bitmap_add_checked(roaring, v)) {
      changed = true;
    }
  }
  if (changed) {
    self->invalidate();
  }
}

void RoaringBitmap32_tryAdd(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (!self) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  if (self->isFrozen()) {
    return v8utils::throwError(isolate, ERROR_FROZEN);
  }

  bool changed = false;
  auto roaring = self->roaring;
  uint32_t v = 0;
  int len = info.Length();
  for (int i = 0; i < len; ++i) {
    auto arg = info[i];
    if (arg->IsInt32()) {
      int32_t n = arg->Int32Value(isolate->GetCurrentContext()).FromJust();
      if (n < 0) {
        continue;
      }
      v = (uint32_t)n;
    } else if (arg->IsUint32()) {
      v = arg->Uint32Value(isolate->GetCurrentContext()).FromJust();
    } else {
      double d;
      if (arg->IsNull() || arg->IsUndefined()) {
        continue;
      }
      if (!arg->NumberValue(isolate->GetCurrentContext()).To(&d) || std::isnan(d)) {
        continue;
      }
      int64_t n = (int64_t)d;
      if (n < 0 || n > UINT32_MAX) {
        continue;
      }
      v = (uint32_t)n;
    }
    if (roaring_bitmap_add_checked(roaring, v)) {
      changed = true;
    }
  }
  if (changed) {
    self->invalidate();
  }
  info.GetReturnValue().Set(changed);
}

void RoaringBitmap32_remove(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (!self) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  if (self->isFrozen()) {
    return v8utils::throwError(isolate, ERROR_FROZEN);
  }
  bool changed = false;
  auto roaring = self->roaring;
  uint32_t v = 0;
  int len = info.Length();
  for (int i = 0; i < len; ++i) {
    auto arg = info[i];
    if (arg->IsInt32()) {
      int32_t n = arg->Int32Value(isolate->GetCurrentContext()).FromJust();
      if (n < 0) {
        continue;
      }
      v = (uint32_t)n;
    } else if (arg->IsUint32()) {
      v = arg->Uint32Value(isolate->GetCurrentContext()).FromJust();
    } else {
      double d;
      if (arg->IsNull() || arg->IsUndefined()) {
        continue;
      }
      if (!arg->NumberValue(isolate->GetCurrentContext()).To(&d) || std::isnan(d)) {
        continue;
      }
      int64_t n = (int64_t)d;
      if (n < 0 || n > UINT32_MAX) {
        continue;
      }
      v = (uint32_t)n;
    }
    if (roaring_bitmap_remove_checked(roaring, v)) {
      changed = true;
    }
  }
  if (changed) {
    self->invalidate();
  }
  info.GetReturnValue().Set(changed);
}

void RoaringBitmap32_addMany(const v8::FunctionCallbackInfo<v8::Value> & info) {
  auto isolate = info.GetIsolate();
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (!self) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  if (self->isFrozen()) {
    return v8utils::throwError(isolate, ERROR_FROZEN);
  }
  if (info.Length() > 0) {
    self->invalidate();
    if (roaringAddMany(isolate, self, info[0])) {
      return info.GetReturnValue().Set(info.Holder());
    }
  }
  v8utils::throwTypeError(isolate, "Uint32Array, RoaringBitmap32 or Iterable<number> expected");
}

void RoaringBitmap32_pop(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (!self) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  if (self->isFrozen()) {
    return v8utils::throwError(isolate, ERROR_FROZEN);
  }
  uint32_t v = roaring_bitmap_maximum(self->roaring);
  bool result = roaring_bitmap_remove_checked(self->roaring, v);
  if (result) {
    self->invalidate();
    info.GetReturnValue().Set(v);
  }
}

void RoaringBitmap32_shift(const v8::FunctionCallbackInfo<v8::Value> & info) {
  auto isolate = info.GetIsolate();
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (!self) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  if (self->isFrozen()) {
    return v8utils::throwError(isolate, ERROR_FROZEN);
  }
  uint32_t v = roaring_bitmap_minimum(self->roaring);
  bool result = roaring_bitmap_remove_checked(self->roaring, v);
  if (result) {
    self->invalidate();
    info.GetReturnValue().Set(v);
  }
}

void RoaringBitmap32_clear(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (!self) {
    return info.GetReturnValue().Set(false);
  }
  if (self->isFrozen()) {
    return v8utils::throwError(isolate, ERROR_FROZEN);
  }
  if (self->roaring && self->roaring->high_low_container.size == 0) {
    info.GetReturnValue().Set(false);
  } else {
    if (self->roaring != nullptr) {
      roaring_bitmap_clear(self->roaring);
      roaring_bitmap_shrink_to_fit(self->roaring);
      self->invalidate();
    }
    info.GetReturnValue().Set(true);
  }
}

void RoaringBitmap32_removeMany(const v8::FunctionCallbackInfo<v8::Value> & info) {
  bool done = false;
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

    if (arg->IsUint32Array() || arg->IsInt32Array()) {
      const v8utils::TypedArrayContent<uint32_t> typedArray(isolate, arg);
      roaring_bitmap_remove_many(self->roaring, typedArray.length, typedArray.data);
      self->invalidate();
      done = true;
    } else {
      AddonData * addonData = self->addonData;
      RoaringBitmap32 * other = ObjectWrap::TryUnwrap<RoaringBitmap32>(arg, isolate);
      if (other != nullptr) {
        roaring_bitmap_andnot_inplace(self->roaring, other->roaring);
        self->invalidate();
        done = true;
      } else {
        v8::Local<v8::Value> argv[] = {arg};
        auto tMaybe = addonData->Uint32Array_from.Get(isolate)->Call(
          isolate->GetCurrentContext(), addonData->Uint32Array.Get(isolate), 1, argv);
        v8::Local<v8::Value> t;
        if (tMaybe.ToLocal(&t)) {
          const v8utils::TypedArrayContent<uint32_t> typedArray(isolate, t);
          roaring_bitmap_remove_many(self->roaring, typedArray.length, typedArray.data);
          self->invalidate();
          done = true;
        } else {
          RoaringBitmap32 tmp(addonData, 0U);
          if (roaringAddMany(isolate, &tmp, arg)) {
            roaring_bitmap_andnot_inplace(self->roaring, tmp.roaring);
            self->invalidate();
            done = true;
          }
        }
      }
    }
  }

  if (done) {
    info.GetReturnValue().Set(info.Holder());
  } else {
    v8utils::throwTypeError(isolate, "Uint32Array, RoaringBitmap32 or Iterable<number> expected");
  }
}

void RoaringBitmap32_andInPlace(const v8::FunctionCallbackInfo<v8::Value> & info) {
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
    RoaringBitmap32 * other = ObjectWrap::TryUnwrap<RoaringBitmap32>(arg, isolate);
    if (other != nullptr) {
      roaring_bitmap_and_inplace(self->roaring, other->roaring);
      self->invalidate();
      return info.GetReturnValue().Set(info.Holder());
    }

    RoaringBitmap32 tmp(self->addonData, 0U);
    if (roaringAddMany(isolate, &tmp, arg)) {
      roaring_bitmap_and_inplace(self->roaring, tmp.roaring);
      self->invalidate();
      return info.GetReturnValue().Set(info.Holder());
    }
  }

  return v8utils::throwTypeError(isolate, "Uint32Array, RoaringBitmap32 or Iterable<number> expected");
}

void RoaringBitmap32_xorInPlace(const v8::FunctionCallbackInfo<v8::Value> & info) {
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
    RoaringBitmap32 * other = ObjectWrap::TryUnwrap<RoaringBitmap32>(arg, isolate);
    if (other != nullptr) {
      roaring_bitmap_xor_inplace(self->roaring, other->roaring);
      self->invalidate();
      return info.GetReturnValue().Set(info.Holder());
    }

    RoaringBitmap32 tmp(self->addonData, 0U);
    roaringAddMany(info.GetIsolate(), &tmp, arg);
    roaring_bitmap_xor_inplace(self->roaring, tmp.roaring);
    self->invalidate();
    return info.GetReturnValue().Set(info.Holder());
  }

  return v8utils::throwTypeError(isolate, "Uint32Array, RoaringBitmap32 or Iterable<number> expected");
}

#endif  // ROARING_NODE_ROARINGBITMAP32_OPS_
