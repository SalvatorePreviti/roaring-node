#include "RoaringBitmap32.h"

void RoaringBitmap32::isSubset(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0, RoaringBitmap32::constructorTemplate);
  info.GetReturnValue().Set(self == other || (self && other && roaring_bitmap_is_subset(&self->roaring, &other->roaring)));
}

void RoaringBitmap32::isStrictSubset(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0, RoaringBitmap32::constructorTemplate);
  info.GetReturnValue().Set(self && other && roaring_bitmap_is_strict_subset(&self->roaring, &other->roaring));
}

void RoaringBitmap32::isEqual(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0, RoaringBitmap32::constructorTemplate);
  info.GetReturnValue().Set(self == other || (self && other && roaring_bitmap_equals(&self->roaring, &other->roaring)));
}

void RoaringBitmap32::intersects(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0, RoaringBitmap32::constructorTemplate);
  info.GetReturnValue().Set(self && other && roaring_bitmap_intersect(&self->roaring, &other->roaring));
}

void RoaringBitmap32::andCardinality(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0, RoaringBitmap32::constructorTemplate);
  info.GetReturnValue().Set(self && other ? (double)roaring_bitmap_and_cardinality(&self->roaring, &other->roaring) : -1);
}

void RoaringBitmap32::orCardinality(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0, RoaringBitmap32::constructorTemplate);
  info.GetReturnValue().Set(self && other ? (double)roaring_bitmap_or_cardinality(&self->roaring, &other->roaring) : -1);
}

void RoaringBitmap32::andNotCardinality(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0, RoaringBitmap32::constructorTemplate);
  info.GetReturnValue().Set(other ? (double)roaring_bitmap_andnot_cardinality(&self->roaring, &other->roaring) : -1);
}

void RoaringBitmap32::xorCardinality(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0, RoaringBitmap32::constructorTemplate);
  info.GetReturnValue().Set(self && other ? (double)roaring_bitmap_xor_cardinality(&self->roaring, &other->roaring) : -1);
}

void RoaringBitmap32::jaccardIndex(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0, RoaringBitmap32::constructorTemplate);
  info.GetReturnValue().Set(self && other ? roaring_bitmap_jaccard_index(&self->roaring, &other->roaring) : -1);
}

inline bool roaringAddMany(v8::Isolate * isolate, RoaringBitmap32 * self, v8::Local<v8::Value> arg, bool replace = false) {
  if (arg.IsEmpty()) {
    return false;
  }

  if (arg->IsNullOrUndefined()) {
    if (replace && self->roaring.high_low_container.containers != nullptr) {
      roaring_bitmap_clear(&self->roaring);
    }
    return true;
  }

  if (!arg->IsObject()) {
    return false;
  }

  if (arg->IsUint32Array() || arg->IsInt32Array()) {
    if (replace && self->roaring.high_low_container.containers != nullptr) {
      roaring_bitmap_clear(&self->roaring);
    }
    const v8utils::TypedArrayContent<uint32_t> typedArray(arg);
    roaring_bitmap_add_many(&self->roaring, typedArray.length, typedArray.data);
    self->invalidate();
    return true;
  }

  RoaringBitmap32 * other = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(arg, RoaringBitmap32::constructorTemplate, isolate);
  if (other) {
    if (other && self != other) {
      if (replace || self->roaring.high_low_container.containers == nullptr) {
        if (self->roaring.high_low_container.containers != nullptr) {
          roaring_bitmap_clear(&self->roaring);
        }
        bool is_ok = ra_copy(&other->roaring.high_low_container, &self->roaring.high_low_container, roaring_bitmap_get_copy_on_write(&other->roaring));
        if (!is_ok) {
          v8utils::throwError("RoaringBitmap32 - Failed to copy bitmap");
        }
      } else {
        roaring_bitmap_or_inplace(&self->roaring, &other->roaring);
      }
      self->invalidate();
    }
    return true;
  }

  v8::Local<v8::Value> argv[] = {arg};
  auto t = JSTypes::Uint32Array_from.Get(isolate)->Call(JSTypes::Uint32Array.Get(isolate), 1, argv);
  if (t.IsEmpty()) {
    return false;
  }

  const v8utils::TypedArrayContent<uint32_t> typedArray(t);
  if (replace) {
    roaring_bitmap_clear(&self->roaring);
  }
  roaring_bitmap_add_many(&self->roaring, typedArray.length, typedArray.data);
  self->invalidate();
  return true;
}

void RoaringBitmap32::copyFrom(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  if (info.Length() == 0 || !roaringAddMany(isolate, self, info[0], true)) {
    return v8utils::throwError("RoaringBitmap32::copyFrom expects a RoaringBitmap32, an Uint32Array or an Iterable");
  }
  self->invalidate();
}

void RoaringBitmap32::addMany(const v8::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() > 0) {
    RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
    self->invalidate();
    if (roaringAddMany(info.GetIsolate(), self, info[0])) {
      return info.GetReturnValue().Set(info.Holder());
    }
  }
  return v8utils::throwTypeError("Uint32Array, RoaringBitmap32 or Iterable<number> expected");
}

void RoaringBitmap32::add(const v8::FunctionCallbackInfo<v8::Value> & info) {
  uint32_t v;
  if (info.Length() < 1 || !info[0]->IsUint32() || !info[0]->Uint32Value(info.GetIsolate()->GetCurrentContext()).To(&v)) {
    return v8utils::throwTypeError("RoaringBitmap32::add - 32 bit unsigned integer expected");
  }

  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  roaring_bitmap_add(&self->roaring, v);
  self->invalidate();
  return info.GetReturnValue().Set(info.Holder());
}

void RoaringBitmap32::tryAdd(const v8::FunctionCallbackInfo<v8::Value> & info) {
  uint32_t v;
  if (info.Length() < 1 || !info[0]->IsUint32() || !info[0]->Uint32Value(info.GetIsolate()->GetCurrentContext()).To(&v)) {
    return info.GetReturnValue().Set(false);
  }

  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  bool result = roaring_bitmap_add_checked(&self->roaring, v);
  if (result) {
    self->invalidate();
  }
  info.GetReturnValue().Set(result);
}

void RoaringBitmap32::removeMany(const v8::FunctionCallbackInfo<v8::Value> & info) {
  bool done = false;

  if (info.Length() > 0) {
    v8::Isolate * isolate = info.GetIsolate();
    auto const & arg = info[0];
    RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());

    if (arg->IsUint32Array() || arg->IsInt32Array()) {
      const v8utils::TypedArrayContent<uint32_t> typedArray(arg);
      roaring_bitmap_remove_many(&self->roaring, typedArray.length, typedArray.data);
      self->invalidate();
      done = true;
    } else {
      RoaringBitmap32 * other = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(arg, RoaringBitmap32::constructorTemplate, isolate);
      if (other) {
        roaring_bitmap_andnot_inplace(&self->roaring, &other->roaring);
        self->invalidate();
        done = true;
      } else {
        v8::Local<v8::Value> argv[] = {arg};
        auto t = JSTypes::Uint32Array_from.Get(isolate)->Call(JSTypes::Uint32Array.Get(isolate), 1, argv);
        if (!t.IsEmpty()) {
          const v8utils::TypedArrayContent<uint32_t> typedArray(t);
          roaring_bitmap_remove_many(&self->roaring, typedArray.length, typedArray.data);
          self->invalidate();
          done = true;
        } else {
          RoaringBitmap32 tmp;
          if (roaringAddMany(isolate, &tmp, arg)) {
            roaring_bitmap_andnot_inplace(&self->roaring, &tmp.roaring);
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
    v8utils::throwTypeError("Uint32Array, RoaringBitmap32 or Iterable<number> expected");
  }
}

void RoaringBitmap32::andInPlace(const v8::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() > 0) {
    v8::Isolate * isolate = info.GetIsolate();
    auto const & arg = info[0];
    RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
    RoaringBitmap32 * other = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(arg, RoaringBitmap32::constructorTemplate, isolate);
    if (other) {
      roaring_bitmap_and_inplace(&self->roaring, &other->roaring);
      self->invalidate();
      return info.GetReturnValue().Set(info.Holder());
    } else {
      RoaringBitmap32 tmp;
      if (roaringAddMany(isolate, &tmp, arg)) {
        roaring_bitmap_and_inplace(&self->roaring, &tmp.roaring);
        self->invalidate();
        return info.GetReturnValue().Set(info.Holder());
      }
    }
  }

  return v8utils::throwTypeError("Uint32Array, RoaringBitmap32 or Iterable<number> expected");
}

void RoaringBitmap32::xorInPlace(const v8::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() > 0) {
    v8::Isolate * isolate = info.GetIsolate();
    auto const & arg = info[0];
    RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
    RoaringBitmap32 * other = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(arg, RoaringBitmap32::constructorTemplate, isolate);
    if (other) {
      roaring_bitmap_xor_inplace(&self->roaring, &other->roaring);
      self->invalidate();
      return info.GetReturnValue().Set(info.Holder());
    } else {
      RoaringBitmap32 tmp;
      roaringAddMany(info.GetIsolate(), &tmp, arg);
      roaring_bitmap_xor_inplace(&self->roaring, &tmp.roaring);
      self->invalidate();
      return info.GetReturnValue().Set(info.Holder());
    }
  }

  return v8utils::throwTypeError("Uint32Array, RoaringBitmap32 or Iterable<number> expected");
}

void RoaringBitmap32::remove(const v8::FunctionCallbackInfo<v8::Value> & info) {
  uint32_t v;
  if (info.Length() >= 1 && info[0]->IsUint32() && info[0]->Uint32Value(info.GetIsolate()->GetCurrentContext()).To(&v)) {
    RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
    roaring_bitmap_remove(&self->roaring, v);
    self->invalidate();
  }
}

void RoaringBitmap32::removeChecked(const v8::FunctionCallbackInfo<v8::Value> & info) {
  uint32_t v;
  if (info.Length() < 1 || !info[0]->IsUint32() || !info[0]->Uint32Value(info.GetIsolate()->GetCurrentContext()).To(&v)) {
    return info.GetReturnValue().Set(false);
  }
  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  bool result = roaring_bitmap_remove_checked(&self->roaring, v);
  if (result) {
    self->invalidate();
  }
  info.GetReturnValue().Set(result);
}

void RoaringBitmap32::clear(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  if (self->roaring.high_low_container.size == 0) {
    info.GetReturnValue().Set(false);
  } else {
    ra_clear(&self->roaring.high_low_container);
    self->roaring.high_low_container = roaring_array_t{};
    self->invalidate();
    info.GetReturnValue().Set(true);
  }
}

inline static bool getRangeOperationParameters(const v8::FunctionCallbackInfo<v8::Value> & info, uint64_t & minInteger, uint64_t & maxInteger) {
  if (info.Length() < 2 || !info[0]->IsNumber() || !info[1]->IsNumber()) {
    return false;
  }

  v8::Isolate * isolate = info.GetIsolate();
  double minimum, maximum;

  if (!info[0]->NumberValue(isolate->GetCurrentContext()).To(&minimum) || std::isnan(minimum)) {
    return false;
  }

  if (!info[1]->NumberValue(isolate->GetCurrentContext()).To(&maximum) || std::isnan(maximum)) {
    return false;
  }

  if (minimum < 0) {
    minimum = 0;
  }

  if (maximum < 0) {
    maximum = 0;
  } else if (maximum > 4294967296) {
    maximum = 4294967296;
  }

  minInteger = (uint64_t)minimum;
  maxInteger = (uint64_t)maximum;

  return minInteger < maxInteger;
}

void RoaringBitmap32::rangeCardinality(const v8::FunctionCallbackInfo<v8::Value> & info) {
  uint64_t minInteger, maxInteger;
  if (getRangeOperationParameters(info, minInteger, maxInteger)) {
    RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
    auto card = roaring_bitmap_range_cardinality(&self->roaring, minInteger, maxInteger);
    if (card <= 0xFFFFFFFF) {
      return info.GetReturnValue().Set((uint32_t)card);
    }
    return info.GetReturnValue().Set((double)card);
  }
  return info.GetReturnValue().Set(0u);
}

void RoaringBitmap32::flipRange(const v8::FunctionCallbackInfo<v8::Value> & info) {
  uint64_t minInteger, maxInteger;
  if (getRangeOperationParameters(info, minInteger, maxInteger)) {
    RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
    roaring_bitmap_flip_inplace(&self->roaring, minInteger, maxInteger);
    self->invalidate();
  }
}

void RoaringBitmap32::addRange(const v8::FunctionCallbackInfo<v8::Value> & info) {
  uint64_t minInteger, maxInteger;
  if (getRangeOperationParameters(info, minInteger, maxInteger)) {
    RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
    roaring_bitmap_add_range_closed(&self->roaring, (uint32_t)minInteger, (uint32_t)(maxInteger - 1));
    self->invalidate();
  }
}

void RoaringBitmap32::removeRange(const v8::FunctionCallbackInfo<v8::Value> & info) {
  uint64_t minInteger, maxInteger;
  if (getRangeOperationParameters(info, minInteger, maxInteger)) {
    RoaringBitmap32 * self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
    roaring_bitmap_remove_range_closed(&self->roaring, (uint32_t)minInteger, (uint32_t)(maxInteger - 1));
    self->invalidate();
  }
}

void RoaringBitmap32::swapStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  if (info.Length() < 2)
    return v8utils::throwTypeError("RoaringBitmap32::swap expects 2 arguments");

  auto constructorTemplate = RoaringBitmap32::constructorTemplate.Get(isolate);

  RoaringBitmap32 * a = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info[0], constructorTemplate, isolate);
  if (!a)
    return v8utils::throwTypeError("RoaringBitmap32::swap first argument must be a RoaringBitmap32");

  RoaringBitmap32 * b = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info[1], constructorTemplate, isolate);
  if (!b)
    return v8utils::throwTypeError("RoaringBitmap32::swap second argument must be a RoaringBitmap32");

  if (a != b) {
    std::swap(a->roaring, b->roaring);
    a->invalidate();
    b->invalidate();
  }
}

void RoaringBitmap32::andStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::EscapableHandleScope scope(isolate);

  auto constructorTemplate = RoaringBitmap32::constructorTemplate.Get(isolate);

  if (info.Length() < 2)
    return v8utils::throwTypeError("RoaringBitmap32::and expects 2 arguments");

  RoaringBitmap32 * a = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info[0], constructorTemplate, isolate);
  if (!a)
    return v8utils::throwTypeError("RoaringBitmap32::and first argument must be a RoaringBitmap32");

  RoaringBitmap32 * b = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info[1], constructorTemplate, isolate);
  if (!b)
    return v8utils::throwTypeError("RoaringBitmap32::and second argument must be a RoaringBitmap32");

  v8::Local<v8::Function> cons = constructor.Get(isolate);

  auto resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
  if (resultMaybe.IsEmpty())
    return;

  auto result = resultMaybe.ToLocalChecked();
  auto self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(result);

  roaring_bitmap_t * r = roaring_bitmap_and(&a->roaring, &b->roaring);
  if (r == nullptr)
    return v8utils::throwTypeError("RoaringBitmap32::and failed materalization");

  ra_clear(&self->roaring.high_low_container);
  self->roaring.high_low_container = std::move(r->high_low_container);

  info.GetReturnValue().Set(scope.Escape(result));
}

void RoaringBitmap32::orStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::EscapableHandleScope scope(isolate);

  if (info.Length() < 2)
    return v8utils::throwTypeError("RoaringBitmap32::or expects 2 arguments");

  auto constructorTemplate = RoaringBitmap32::constructorTemplate.Get(isolate);

  RoaringBitmap32 * a = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info[0], constructorTemplate, isolate);
  if (!a)
    return v8utils::throwTypeError("RoaringBitmap32::or first argument must be a RoaringBitmap32");

  RoaringBitmap32 * b = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info[1], constructorTemplate, isolate);
  if (!b)
    return v8utils::throwTypeError("RoaringBitmap32::or second argument must be a RoaringBitmap32");

  v8::Local<v8::Function> cons = constructor.Get(isolate);

  auto resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
  if (resultMaybe.IsEmpty())
    return;

  auto result = resultMaybe.ToLocalChecked();
  auto self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(result);

  roaring_bitmap_t * r = roaring_bitmap_or(&a->roaring, &b->roaring);
  if (r == nullptr)
    return v8utils::throwTypeError("RoaringBitmap32::or failed materalization");

  ra_clear(&self->roaring.high_low_container);
  self->roaring.high_low_container = std::move(r->high_low_container);

  info.GetReturnValue().Set(scope.Escape(result));
}

void RoaringBitmap32::xorStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::EscapableHandleScope scope(isolate);

  if (info.Length() < 2)
    return v8utils::throwTypeError("RoaringBitmap32::xor expects 2 arguments");

  auto constructorTemplate = RoaringBitmap32::constructorTemplate.Get(isolate);

  RoaringBitmap32 * a = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info[0], constructorTemplate, isolate);
  if (!a)
    return v8utils::throwTypeError("RoaringBitmap32::xor first argument must be a RoaringBitmap32");

  RoaringBitmap32 * b = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info[1], constructorTemplate, isolate);
  if (!b)
    return v8utils::throwTypeError("RoaringBitmap32::xor second argument must be a RoaringBitmap32");

  v8::Local<v8::Function> cons = constructor.Get(isolate);

  auto resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
  if (resultMaybe.IsEmpty())
    return;

  auto result = resultMaybe.ToLocalChecked();
  auto self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(result);

  roaring_bitmap_t * r = roaring_bitmap_xor(&a->roaring, &b->roaring);
  if (r == nullptr)
    return v8utils::throwTypeError("RoaringBitmap32::xor failed materalization");

  ra_clear(&self->roaring.high_low_container);
  self->roaring.high_low_container = std::move(r->high_low_container);

  info.GetReturnValue().Set(scope.Escape(result));
}

void RoaringBitmap32::andNotStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::EscapableHandleScope scope(isolate);

  if (info.Length() < 2)
    return v8utils::throwTypeError("RoaringBitmap32::andnot expects 2 arguments");

  auto constructorTemplate = RoaringBitmap32::constructorTemplate.Get(isolate);

  RoaringBitmap32 * a = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info[0], constructorTemplate, isolate);
  if (!a)
    return v8utils::throwTypeError("RoaringBitmap32::andnot first argument must be a RoaringBitmap32");

  RoaringBitmap32 * b = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info[1], constructorTemplate, isolate);
  if (!b)
    return v8utils::throwTypeError("RoaringBitmap32::andnot second argument must be a RoaringBitmap32");

  v8::Local<v8::Function> cons = constructor.Get(isolate);

  auto resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
  if (resultMaybe.IsEmpty())
    return;

  auto result = resultMaybe.ToLocalChecked();
  auto self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(result);

  roaring_bitmap_t * r = roaring_bitmap_andnot(&a->roaring, &b->roaring);
  if (r == nullptr)
    return v8utils::throwTypeError("RoaringBitmap32::andnot failed materalization");

  ra_clear(&self->roaring.high_low_container);
  self->roaring.high_low_container = std::move(r->high_low_container);

  info.GetReturnValue().Set(scope.Escape(result));
}

void RoaringBitmap32::fromRangeStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  v8::EscapableHandleScope scope(isolate);

  v8::Local<v8::Function> cons = RoaringBitmap32::constructor.Get(isolate);

  auto resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
  if (resultMaybe.IsEmpty()) {
    return;
  }

  uint32_t v;
  uint32_t step = 1;
  if (info.Length() >= 3 && info[2]->IsUint32() && info[2]->Uint32Value(isolate->GetCurrentContext()).To(&v)) {
    step = v;
    if (step == 0) {
      step = 1;
    }
  }

  auto result = resultMaybe.ToLocalChecked();
  auto self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(result);

  uint64_t minInteger, maxInteger;
  if (getRangeOperationParameters(info, minInteger, maxInteger)) {
    roaring_bitmap_t * temp = roaring_bitmap_from_range(minInteger, maxInteger, step);
    if (temp != nullptr) {
      self->roaring = *temp;
      free(temp);
    }
  }

  info.GetReturnValue().Set(scope.Escape(result));
}

template <typename TSize>
void RoaringBitmap32_opManyStatic(const char * opName,
    roaring_bitmap_t * op(TSize number, const roaring_bitmap_t ** x),
    const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::EscapableHandleScope scope(isolate);

  int length = info.Length();

  v8::Local<v8::FunctionTemplate> ctorType = RoaringBitmap32::constructorTemplate.Get(isolate);
  v8::Local<v8::Function> cons = RoaringBitmap32::constructor.Get(isolate);

  if (length == 0) {
    auto v = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
    if (!v.IsEmpty()) {
      info.GetReturnValue().Set(scope.Escape(v.ToLocalChecked()));
    }
    return;
  }

  if (length == 1) {
    if (info[0]->IsArray()) {
      auto array = v8::Local<v8::Array>::Cast(info[0]);

      size_t arrayLength = array->Length();

      if (arrayLength == 0) {
        auto v = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
        if (!v.IsEmpty()) {
          info.GetReturnValue().Set(scope.Escape(v.ToLocalChecked()));
        }
        return;
      }

      if (arrayLength == 1) {
        v8::Local<v8::Value> item = array->Get(0);
        if (!ctorType->HasInstance(item)) {
          return v8utils::throwTypeError(std::string(opName) + " accepts only RoaringBitmap32 instances");
        }

        v8::Local<v8::Value> argv[] = {item};
        auto v = cons->NewInstance(isolate->GetCurrentContext(), 1, argv);
        if (!v.IsEmpty()) {
          info.GetReturnValue().Set(scope.Escape(v.ToLocalChecked()));
        }
        return;
      }

      const roaring_bitmap_t ** x = (const roaring_bitmap_t **)malloc(arrayLength * sizeof(roaring_bitmap_t *));
      if (x == nullptr) {
        return v8utils::throwTypeError(std::string(opName) + " failed allocation");
      }

      for (size_t i = 0; i < arrayLength; ++i) {
        v8::Local<v8::Value> item = array->Get(i);
        if (!ctorType->HasInstance(item)) {
          return v8utils::throwTypeError(std::string(opName) + " accepts only RoaringBitmap32 instances");
        }
        RoaringBitmap32 * p = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(item, ctorType, isolate);
        if (p == nullptr) {
          free(x);
          return v8utils::throwTypeError(std::string(opName) + " accepts only RoaringBitmap32 instances");
        }
        x[i] = &p->roaring;
      }

      auto resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
      if (resultMaybe.IsEmpty()) {
        free(x);
        return;
      }

      auto result = resultMaybe.ToLocalChecked();
      auto self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(result);

      ra_clear(&self->roaring.high_low_container);
      self->roaring.high_low_container = roaring_array_t{};

      roaring_bitmap_t * r = op(arrayLength, x);
      if (r == nullptr) {
        free(x);
        return v8utils::throwTypeError(std::string(opName) + " failed roaring allocation");
      }

      self->roaring.high_low_container = std::move(r->high_low_container);

      info.GetReturnValue().Set(scope.Escape(result));

    } else {
      if (!ctorType->HasInstance(info[0])) {
        return v8utils::throwTypeError(std::string(opName) + " accepts only RoaringBitmap32 instances");
      }

      v8::Local<v8::Value> argv[] = {info[0]};
      auto v = cons->NewInstance(isolate->GetCurrentContext(), 1, argv);
      if (!v.IsEmpty()) {
        info.GetReturnValue().Set(scope.Escape(v.ToLocalChecked()));
      }
    }
  } else {
    const roaring_bitmap_t ** x = (const roaring_bitmap_t **)malloc(length * sizeof(roaring_bitmap_t *));
    if (x == nullptr) {
      return v8utils::throwTypeError(std::string(opName) + " failed allocation");
    }

    for (int i = 0; i < length; ++i) {
      RoaringBitmap32 * p = v8utils::ObjectWrap::TryUnwrap<RoaringBitmap32>(info, i, ctorType);
      if (p == nullptr) {
        free(x);
        return v8utils::throwTypeError(std::string(opName) + " accepts only RoaringBitmap32 instances");
      }
      x[i] = &p->roaring;
    }

    v8::MaybeLocal<v8::Object> resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
    if (resultMaybe.IsEmpty()) {
      free(x);
      return;
    }

    v8::Local<v8::Object> result = resultMaybe.ToLocalChecked();
    auto self = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(result);

    ra_clear(&self->roaring.high_low_container);
    self->roaring.high_low_container = roaring_array_t{};

    roaring_bitmap_t * r = op(length, x);
    if (r == nullptr) {
      free(x);
      return v8utils::throwTypeError(std::string(opName) + " failed roaring allocation");
    }

    self->roaring.high_low_container = std::move(r->high_low_container);

    info.GetReturnValue().Set(scope.Escape(result));
  }
}

void RoaringBitmap32::orManyStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32_opManyStatic("RoaringBitmap32::orMany", roaring_bitmap_or_many_heap, info);
}

void RoaringBitmap32::xorManyStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32_opManyStatic("RoaringBitmap32::xorMany", roaring_bitmap_xor_many, info);
}

struct FromArrayAsyncWorker : public RoaringBitmap32FactoryAsyncWorker {
 public:
  v8::Persistent<v8::Value> argPersistent;
  v8utils::TypedArrayContent<uint32_t> buffer;

  FromArrayAsyncWorker(v8::Isolate * isolate) : RoaringBitmap32FactoryAsyncWorker(isolate) {
  }

  virtual ~FromArrayAsyncWorker() {
    argPersistent.Reset();
  }

 protected:
  virtual void work() {
    roaring_bitmap_add_many(&bitmap, buffer.length, buffer.data);
    roaring_bitmap_run_optimize(&bitmap);
    roaring_bitmap_shrink_to_fit(&bitmap);
  }
};

void RoaringBitmap32::fromArrayStaticAsync(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  v8::EscapableHandleScope scope(isolate);

  v8::Local<v8::Value> arg;

  if (info.Length() >= 1) {
    arg = info[0];
  }

  FromArrayAsyncWorker * worker = new FromArrayAsyncWorker(isolate);
  if (worker == nullptr) {
    return v8utils::throwError("Failed to allocate async worker");
  }

  if (!arg->IsNullOrUndefined() && !arg->IsFunction()) {
    if (arg->IsUint32Array() || arg->IsInt32Array()) {
      worker->buffer.set(arg);
      const v8utils::TypedArrayContent<uint32_t> typedArray(arg);
    } else if (arg->IsObject()) {
      if (RoaringBitmap32::constructorTemplate.Get(isolate)->HasInstance(arg)) {
        return v8utils::throwTypeError("RoaringBitmap32::fromArrayAsync cannot be called with a RoaringBitmap32 instance");
      } else {
        v8::Local<v8::Value> argv[] = {arg};
        arg = JSTypes::Uint32Array_from.Get(isolate)->Call(JSTypes::Uint32Array.Get(isolate), 1, argv);
        if (arg.IsEmpty()) {
          return;
        }
        worker->buffer.set(arg);
      }
    }

    worker->argPersistent.Reset(isolate, arg);
  }

  if (info.Length() >= 2 && info[1]->IsFunction()) {
    worker->setCallback(info[1]);
  } else if (info.Length() >= 1 && info[0]->IsFunction()) {
    worker->setCallback(info[0]);
  }

  v8::Local<v8::Value> returnValue = v8utils::AsyncWorker::run(worker);
  info.GetReturnValue().Set(scope.Escape(returnValue));
}