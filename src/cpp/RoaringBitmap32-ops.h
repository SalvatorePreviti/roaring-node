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

  if (arg->IsUint32Array() || arg->IsInt32Array() || arg->IsArrayBuffer()) {
    if (replace && self->roaring->high_low_container.containers != nullptr) {
      roaring_bitmap_clear(self->roaring);
    }
    const v8utils::TypedArrayContent<uint32_t> typedArray(isolate, arg);
    roaring_bitmap_add_many(self->roaring, typedArray.length, typedArray.data);
    self->invalidate();
    return true;
  }

  RoaringBitmap32 * other =
    ObjectWrap::TryUnwrap<RoaringBitmap32>(arg, globalAddonData.RoaringBitmap32_constructorTemplate, isolate);
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

  v8::Local<v8::Value> argv[] = {arg};
  auto tMaybe = globalAddonData.Uint32Array_from.Get(isolate)->Call(
    isolate->GetCurrentContext(), globalAddonData.Uint32Array.Get(isolate), 1, argv);
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

void RoaringBitmap32::andCardinality(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  RoaringBitmap32 * other =
    ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0, globalAddonData.RoaringBitmap32_constructorTemplate);
  info.GetReturnValue().Set(self && other ? (double)roaring_bitmap_and_cardinality(self->roaring, other->roaring) : -1);
}

void RoaringBitmap32::orCardinality(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  RoaringBitmap32 * other =
    ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0, globalAddonData.RoaringBitmap32_constructorTemplate);
  info.GetReturnValue().Set(self && other ? (double)roaring_bitmap_or_cardinality(self->roaring, other->roaring) : -1);
}

void RoaringBitmap32::andNotCardinality(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  RoaringBitmap32 * other =
    ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0, globalAddonData.RoaringBitmap32_constructorTemplate);
  info.GetReturnValue().Set(self && other ? (double)roaring_bitmap_andnot_cardinality(self->roaring, other->roaring) : -1);
}

void RoaringBitmap32::xorCardinality(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), info.GetIsolate());
  if (self == nullptr) {
    return info.GetReturnValue().Set(0);
  }
  RoaringBitmap32 * other =
    ObjectWrap::TryUnwrap<RoaringBitmap32>(info, 0, globalAddonData.RoaringBitmap32_constructorTemplate);
  info.GetReturnValue().Set(self && other ? (double)roaring_bitmap_xor_cardinality(self->roaring, other->roaring) : -1);
}

void RoaringBitmap32::add(const v8::FunctionCallbackInfo<v8::Value> & info) {
  uint32_t v;
  auto isolate = info.GetIsolate();

  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (!self) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  if (self->isFrozen()) {
    return v8utils::throwError(isolate, ERROR_FROZEN);
  }

  if (info.Length() < 1 || !info[0]->IsUint32() || !info[0]->Uint32Value(isolate->GetCurrentContext()).To(&v)) {
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::add - 32 bit unsigned integer expected");
  }

  roaring_bitmap_add(self->roaring, v);
  self->invalidate();
  info.GetReturnValue().Set(info.Holder());
}

void RoaringBitmap32::tryAdd(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (!self) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  if (self->isFrozen()) {
    return v8utils::throwError(isolate, ERROR_FROZEN);
  }

  uint32_t v;
  if (info.Length() < 1 || !info[0]->IsUint32() || !info[0]->Uint32Value(isolate->GetCurrentContext()).To(&v)) {
    return info.GetReturnValue().Set(false);
  }

  bool result = roaring_bitmap_add_checked(self->roaring, v);
  if (result) {
    self->invalidate();
  }
  info.GetReturnValue().Set(result);
}

void RoaringBitmap32::addMany(const v8::FunctionCallbackInfo<v8::Value> & info) {
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

void RoaringBitmap32::remove(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (!self) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  if (self->isFrozen()) {
    return v8utils::throwError(isolate, ERROR_FROZEN);
  }
  uint32_t v;
  if (info.Length() >= 1 && info[0]->IsUint32() && info[0]->Uint32Value(isolate->GetCurrentContext()).To(&v)) {
    roaring_bitmap_remove(self->roaring, v);
    self->invalidate();
  }
}

void RoaringBitmap32::removeChecked(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (!self) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }
  if (self->isFrozen()) {
    return v8utils::throwError(isolate, ERROR_FROZEN);
  }
  uint32_t v;
  if (info.Length() < 1 || !info[0]->IsUint32() || !info[0]->Uint32Value(isolate->GetCurrentContext()).To(&v)) {
    return info.GetReturnValue().Set(false);
  }
  bool result = roaring_bitmap_remove_checked(self->roaring, v);
  if (result) {
    self->invalidate();
  }
  info.GetReturnValue().Set(result);
}

void RoaringBitmap32::clear(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  RoaringBitmap32 * self = ObjectWrap::TryUnwrap<RoaringBitmap32>(info.Holder(), isolate);
  if (!self) {
    info.GetReturnValue().Set(false);
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

void RoaringBitmap32::removeMany(const v8::FunctionCallbackInfo<v8::Value> & info) {
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
      RoaringBitmap32 * other =
        ObjectWrap::TryUnwrap<RoaringBitmap32>(arg, globalAddonData.RoaringBitmap32_constructorTemplate, isolate);
      if (other != nullptr) {
        roaring_bitmap_andnot_inplace(self->roaring, other->roaring);
        self->invalidate();
        done = true;
      } else {
        v8::Local<v8::Value> argv[] = {arg};
        auto tMaybe = globalAddonData.Uint32Array_from.Get(isolate)->Call(
          isolate->GetCurrentContext(), globalAddonData.Uint32Array.Get(isolate), 1, argv);
        v8::Local<v8::Value> t;
        if (tMaybe.ToLocal(&t)) {
          const v8utils::TypedArrayContent<uint32_t> typedArray(isolate, t);
          roaring_bitmap_remove_many(self->roaring, typedArray.length, typedArray.data);
          self->invalidate();
          done = true;
        } else {
          RoaringBitmap32 tmp(&globalAddonData, 0U);
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

void RoaringBitmap32::addOffsetStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  auto constructorTemplate = globalAddonData.RoaringBitmap32_constructorTemplate.Get(isolate);

  if (info.Length() < 2) return v8utils::throwTypeError(isolate, "RoaringBitmap32::addOffset expects 2 arguments");

  RoaringBitmap32 * a = ObjectWrap::TryUnwrap<RoaringBitmap32>(info[0], constructorTemplate, isolate);
  if (a == nullptr)
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::addOffset first argument must be a RoaringBitmap32");

  v8::Local<v8::Function> cons = globalAddonData.RoaringBitmap32_constructor.Get(isolate);

  auto resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
  v8::Local<v8::Object> result;
  if (!resultMaybe.ToLocal(&result)) {
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::addOffset failed to create new instance");
  }

  auto self = ObjectWrap::TryUnwrap<RoaringBitmap32>(result, isolate);
  if (!self) {
    return v8utils::throwError(info.GetIsolate(), ERROR_INVALID_OBJECT);
  }

  // Second argument is the offset, read as double
  double offset = info[1]->NumberValue(isolate->GetCurrentContext()).FromMaybe(NAN);
  if (std::isnan(offset)) {
    offset = 0;
  } else if (offset < -4294967296) {
    offset = -4294967296;
  } else if (offset > 4294967296) {
    offset = 4294967296;
  }

  roaring_bitmap_t * r = roaring_bitmap_add_offset(a->roaring, (int64_t)offset);
  if (r == nullptr) {
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::addOffset failed materalization");
  }
  self->replaceBitmapInstance(isolate, r);

  info.GetReturnValue().Set(result);
}

void RoaringBitmap32::andStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  auto constructorTemplate = globalAddonData.RoaringBitmap32_constructorTemplate.Get(isolate);

  if (info.Length() < 2) return v8utils::throwTypeError(isolate, "RoaringBitmap32::and expects 2 arguments");

  RoaringBitmap32 * a = ObjectWrap::TryUnwrap<RoaringBitmap32>(info[0], constructorTemplate, isolate);
  if (a == nullptr) return v8utils::throwTypeError(isolate, "RoaringBitmap32::and first argument must be a RoaringBitmap32");

  RoaringBitmap32 * b = ObjectWrap::TryUnwrap<RoaringBitmap32>(info[1], constructorTemplate, isolate);
  if (b == nullptr)
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::and second argument must be a RoaringBitmap32");

  v8::Local<v8::Function> cons = globalAddonData.RoaringBitmap32_constructor.Get(isolate);

  auto resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
  v8::Local<v8::Object> result;
  if (!resultMaybe.ToLocal(&result)) {
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::and failed to create new instance");
  }

  auto self = ObjectWrap::TryUnwrap<RoaringBitmap32>(result, isolate);
  if (!self) {
    return v8utils::throwError(info.GetIsolate(), ERROR_INVALID_OBJECT);
  }

  roaring_bitmap_t * r = roaring_bitmap_and(a->roaring, b->roaring);
  if (r == nullptr) {
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::and failed materalization");
  }

  self->replaceBitmapInstance(isolate, r);

  info.GetReturnValue().Set(result);
}

void RoaringBitmap32::orStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  if (info.Length() < 2) return v8utils::throwTypeError(isolate, "RoaringBitmap32::or expects 2 arguments");

  auto constructorTemplate = globalAddonData.RoaringBitmap32_constructorTemplate.Get(isolate);

  RoaringBitmap32 * a = ObjectWrap::TryUnwrap<RoaringBitmap32>(info[0], constructorTemplate, isolate);
  if (a == nullptr) return v8utils::throwTypeError(isolate, "RoaringBitmap32::or first argument must be a RoaringBitmap32");

  RoaringBitmap32 * b = ObjectWrap::TryUnwrap<RoaringBitmap32>(info[1], constructorTemplate, isolate);
  if (b == nullptr) return v8utils::throwTypeError(isolate, "RoaringBitmap32::or second argument must be a RoaringBitmap32");

  v8::Local<v8::Function> cons = globalAddonData.RoaringBitmap32_constructor.Get(isolate);

  auto resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
  v8::Local<v8::Object> result;
  if (!resultMaybe.ToLocal(&result)) return;

  auto self = ObjectWrap::TryUnwrap<RoaringBitmap32>(result, isolate);
  if (!self) {
    return v8utils::throwError(info.GetIsolate(), ERROR_INVALID_OBJECT);
  }

  roaring_bitmap_t * r = roaring_bitmap_or(a->roaring, b->roaring);
  if (r == nullptr) return v8utils::throwTypeError(isolate, "RoaringBitmap32::or failed materalization");

  self->replaceBitmapInstance(isolate, r);

  info.GetReturnValue().Set(result);
}

void RoaringBitmap32::xorStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  if (info.Length() < 2) return v8utils::throwTypeError(isolate, "RoaringBitmap32::xor expects 2 arguments");

  auto constructorTemplate = globalAddonData.RoaringBitmap32_constructorTemplate.Get(isolate);

  RoaringBitmap32 * a = ObjectWrap::TryUnwrap<RoaringBitmap32>(info[0], constructorTemplate, isolate);
  if (a == nullptr) return v8utils::throwTypeError(isolate, "RoaringBitmap32::xor first argument must be a RoaringBitmap32");

  RoaringBitmap32 * b = ObjectWrap::TryUnwrap<RoaringBitmap32>(info[1], constructorTemplate, isolate);
  if (b == nullptr)
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::xor second argument must be a RoaringBitmap32");

  v8::Local<v8::Function> cons = globalAddonData.RoaringBitmap32_constructor.Get(isolate);

  auto resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
  v8::Local<v8::Object> result;
  if (!resultMaybe.ToLocal(&result)) return;

  auto self = ObjectWrap::TryUnwrap<RoaringBitmap32>(result, isolate);
  if (!self) {
    return v8utils::throwError(info.GetIsolate(), ERROR_INVALID_OBJECT);
  }

  roaring_bitmap_t * r = roaring_bitmap_xor(a->roaring, b->roaring);
  if (r == nullptr) return v8utils::throwTypeError(isolate, "RoaringBitmap32::xor failed materalization");

  self->replaceBitmapInstance(isolate, r);

  info.GetReturnValue().Set(result);
}

void RoaringBitmap32::andNotStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  if (info.Length() < 2) return v8utils::throwTypeError(isolate, "RoaringBitmap32::andnot expects 2 arguments");

  auto constructorTemplate = globalAddonData.RoaringBitmap32_constructorTemplate.Get(isolate);

  RoaringBitmap32 * a = ObjectWrap::TryUnwrap<RoaringBitmap32>(info[0], constructorTemplate, isolate);
  if (a == nullptr)
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::andnot first argument must be a RoaringBitmap32");

  RoaringBitmap32 * b = ObjectWrap::TryUnwrap<RoaringBitmap32>(info[1], constructorTemplate, isolate);
  if (b == nullptr)
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::andnot second argument must be a RoaringBitmap32");

  v8::Local<v8::Function> cons = globalAddonData.RoaringBitmap32_constructor.Get(isolate);

  auto resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
  v8::Local<v8::Object> result;
  if (!resultMaybe.ToLocal(&result)) return;

  auto self = ObjectWrap::TryUnwrap<RoaringBitmap32>(result, isolate);
  if (!self) {
    return v8utils::throwError(info.GetIsolate(), ERROR_INVALID_OBJECT);
  }

  roaring_bitmap_t * r = roaring_bitmap_andnot(a->roaring, b->roaring);
  if (r == nullptr) {
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::andnot failed materalization");
  }

  self->replaceBitmapInstance(isolate, r);

  info.GetReturnValue().Set(result);
}

template <typename TSize>
void RoaringBitmap32_opManyStatic(
  const char * opName,
  roaring_bitmap_t * op(TSize number, const roaring_bitmap_t ** x),
  const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  int length = info.Length();

  v8::Local<v8::FunctionTemplate> ctorType = globalAddonData.RoaringBitmap32_constructorTemplate.Get(isolate);
  v8::Local<v8::Function> cons = globalAddonData.RoaringBitmap32_constructor.Get(isolate);

  auto context = isolate->GetCurrentContext();

  if (length == 0) {
    auto vMaybe = cons->NewInstance(context, 0, nullptr);
    v8::Local<v8::Object> v;
    if (vMaybe.ToLocal(&v)) {
      info.GetReturnValue().Set(v);
    }
    return;
  }

  if (length == 1) {
    if (info[0]->IsArray()) {
      auto array = v8::Local<v8::Array>::Cast(info[0]);

      size_t arrayLength = array->Length();

      if (arrayLength == 0) {
        auto vMaybe = cons->NewInstance(context, 0, nullptr);
        v8::Local<v8::Object> v;
        if (vMaybe.ToLocal(&v)) {
          info.GetReturnValue().Set(v);
        }
        return;
      }

      if (arrayLength == 1) {
        auto itemMaybe = array->Get(context, 0);

        v8::Local<v8::Value> item;
        if (!itemMaybe.ToLocal(&item) || !ctorType->HasInstance(item)) {
          return v8utils::throwTypeError(isolate, opName, " accepts only RoaringBitmap32 instances");
        }

        v8::Local<v8::Value> argv[] = {item};
        auto vMaybe = cons->NewInstance(context, 1, argv);
        v8::Local<v8::Object> v;
        if (vMaybe.ToLocal(&v)) {
          info.GetReturnValue().Set(v);
        }
        return;
      }

      const auto ** x = (const roaring_bitmap_t **)gcaware_malloc(arrayLength * sizeof(roaring_bitmap_t *));
      if (x == nullptr) {
        return v8utils::throwTypeError(isolate, opName, " failed allocation");
      }

      for (size_t i = 0; i < arrayLength; ++i) {
        v8::Local<v8::Value> item;
        auto itemMaybe = array->Get(context, i);
        if (!itemMaybe.ToLocal(&item) || !ctorType->HasInstance(item)) {
          return v8utils::throwTypeError(isolate, opName, " accepts only RoaringBitmap32 instances");
        }
        RoaringBitmap32 * p = ObjectWrap::TryUnwrap<RoaringBitmap32>(item, ctorType, isolate);
        if (p == nullptr) {
          gcaware_free(x);
          return v8utils::throwTypeError(isolate, opName, " accepts only RoaringBitmap32 instances");
        }
        x[i] = p->roaring;
      }

      auto resultMaybe = cons->NewInstance(context, 0, nullptr);
      v8::Local<v8::Object> result;
      if (!resultMaybe.ToLocal(&result)) {
        gcaware_free(x);
        return;
      }

      auto self = ObjectWrap::TryUnwrap<RoaringBitmap32>(result, isolate);
      if (!self) {
        return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
      }

      roaring_bitmap_t * r = op((TSize)arrayLength, x);
      if (r == nullptr) {
        gcaware_free(x);
        return v8utils::throwTypeError(isolate, opName, " failed roaring allocation");
      }

      self->replaceBitmapInstance(isolate, r);

      info.GetReturnValue().Set(result);

    } else {
      if (!ctorType->HasInstance(info[0])) {
        return v8utils::throwTypeError(isolate, opName, " accepts only RoaringBitmap32 instances");
      }

      v8::Local<v8::Value> argv[] = {info[0]};
      auto vMaybe = cons->NewInstance(isolate->GetCurrentContext(), 1, argv);
      v8::Local<v8::Object> v;
      if (vMaybe.ToLocal(&v)) {
        info.GetReturnValue().Set(v);
      }
    }
  } else {
    const auto ** x = (const roaring_bitmap_t **)gcaware_malloc(length * sizeof(roaring_bitmap_t *));
    if (x == nullptr) {
      return v8utils::throwTypeError(isolate, opName, " failed allocation");
    }

    for (int i = 0; i < length; ++i) {
      RoaringBitmap32 * p = ObjectWrap::TryUnwrap<RoaringBitmap32>(info, i, ctorType);
      if (p == nullptr) {
        gcaware_free(x);
        return v8utils::throwTypeError(isolate, opName, " accepts only RoaringBitmap32 instances");
      }
      x[i] = p->roaring;
    }

    v8::MaybeLocal<v8::Object> resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
    v8::Local<v8::Object> result;
    if (!resultMaybe.ToLocal(&result)) {
      gcaware_free(x);
      return;
    }

    auto self = ObjectWrap::TryUnwrap<RoaringBitmap32>(result, isolate);
    if (!self) {
      return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
    }

    roaring_bitmap_t * r = op((TSize)length, x);
    if (r == nullptr) {
      gcaware_free(x);
      return v8utils::throwTypeError(isolate, opName, " failed roaring allocation");
    }

    self->replaceBitmapInstance(isolate, r);

    info.GetReturnValue().Set(result);
  }
}

void RoaringBitmap32::orManyStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32_opManyStatic("RoaringBitmap32::orMany", roaring_bitmap_or_many_heap, info);
}

void RoaringBitmap32::xorManyStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32_opManyStatic("RoaringBitmap32::xorMany", roaring_bitmap_xor_many, info);
}

#endif  // ROARING_NODE_ROARINGBITMAP32_OPS_
