#ifndef ROARING_NODE_ROARINGBITMAP32_STATIC_OPS_
#define ROARING_NODE_ROARINGBITMAP32_STATIC_OPS_

#include "RoaringBitmap32.h"

void RoaringBitmap32_addOffsetStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();

  if (info.Length() < 2) {
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::addOffset expects 2 arguments");
  }

  RoaringBitmap32 * a = ObjectWrap::TryUnwrap<RoaringBitmap32>(info[0], isolate);
  if (a == nullptr)
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::addOffset first argument must be a RoaringBitmap32");
  AddonData * addonData = a->addonData;
  v8::Local<v8::Function> cons = addonData->RoaringBitmap32_constructor.Get(isolate);

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

void RoaringBitmap32_andStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();

  if (info.Length() < 2) return v8utils::throwTypeError(isolate, "RoaringBitmap32::and expects 2 arguments");

  RoaringBitmap32 * const a = ObjectWrap::TryUnwrap<RoaringBitmap32>(info[0], isolate);
  if (a == nullptr) return v8utils::throwTypeError(isolate, "RoaringBitmap32::and first argument must be a RoaringBitmap32");

  const RoaringBitmap32 * const b = ObjectWrap::TryUnwrap<RoaringBitmap32>(info[1], isolate);
  if (b == nullptr)
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::and second argument must be a RoaringBitmap32");

  AddonData * addonData = a->addonData;
  v8::Local<v8::Function> cons = addonData->RoaringBitmap32_constructor.Get(isolate);

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

void RoaringBitmap32_orStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();

  if (info.Length() < 2) return v8utils::throwTypeError(isolate, "RoaringBitmap32::or expects 2 arguments");

  RoaringBitmap32 * a = ObjectWrap::TryUnwrap<RoaringBitmap32>(info[0], isolate);
  if (a == nullptr) {
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::or first argument must be a RoaringBitmap32");
  }

  const RoaringBitmap32 * b = ObjectWrap::TryUnwrap<RoaringBitmap32>(info[1], isolate);
  if (b == nullptr) {
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::or second argument must be a RoaringBitmap32");
  }

  AddonData * addonData = a->addonData;
  v8::Local<v8::Function> cons = addonData->RoaringBitmap32_constructor.Get(isolate);

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

void RoaringBitmap32_xorStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();

  if (info.Length() < 2) {
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::xor expects 2 arguments");
  }

  RoaringBitmap32 * a = ObjectWrap::TryUnwrap<RoaringBitmap32>(info[0], isolate);
  if (a == nullptr) {
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::xor first argument must be a RoaringBitmap32");
  }

  const RoaringBitmap32 * b = ObjectWrap::TryUnwrap<RoaringBitmap32>(info[1], isolate);
  if (b == nullptr) {
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::xor second argument must be a RoaringBitmap32");
  }

  AddonData * addonData = a->addonData;
  v8::Local<v8::Function> cons = addonData->RoaringBitmap32_constructor.Get(isolate);

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

void RoaringBitmap32_andNotStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();

  if (info.Length() < 2) return v8utils::throwTypeError(isolate, "RoaringBitmap32::andnot expects 2 arguments");

  RoaringBitmap32 * a = ObjectWrap::TryUnwrap<RoaringBitmap32>(info[0], isolate);
  if (a == nullptr) {
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::andnot first argument must be a RoaringBitmap32");
  }

  const RoaringBitmap32 * b = ObjectWrap::TryUnwrap<RoaringBitmap32>(info[1], isolate);
  if (b == nullptr) {
    return v8utils::throwTypeError(isolate, "RoaringBitmap32::andnot second argument must be a RoaringBitmap32");
  }

  AddonData * addonData = a->addonData;
  v8::Local<v8::Function> cons = addonData->RoaringBitmap32_constructor.Get(isolate);

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
void roaringOpMany(
  const char * opName,
  roaring_bitmap_t * op(TSize number, const roaring_bitmap_t ** x),
  const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();

  AddonData * addonData = AddonData::get(info);
  if (addonData == nullptr) {
    return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
  }

  int length = info.Length();

  auto context = isolate->GetCurrentContext();

  v8::Local<v8::Function> cons = addonData->RoaringBitmap32_constructor.Get(isolate);

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
        if (!itemMaybe.ToLocal(&item)) {
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

      auto resultMaybe = cons->NewInstance(context, 0, nullptr);
      v8::Local<v8::Object> result;
      if (!resultMaybe.ToLocal(&result)) {
        return;
      }

      auto self = ObjectWrap::TryUnwrap<RoaringBitmap32>(result, isolate);
      if (!self) {
        return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
      }

      const auto ** x = (const roaring_bitmap_t **)gcaware_malloc(arrayLength * sizeof(roaring_bitmap_t *));
      if (x == nullptr) {
        return v8utils::throwTypeError(isolate, opName, " failed allocation");
      }

      for (size_t i = 0; i < arrayLength; ++i) {
        v8::Local<v8::Value> item;
        const RoaringBitmap32 * p =
          array->Get(context, i).ToLocal(&item) ? ObjectWrap::TryUnwrap<RoaringBitmap32>(item, isolate) : nullptr;
        if (p == nullptr) {
          gcaware_free(x);
          return v8utils::throwTypeError(isolate, opName, " accepts only RoaringBitmap32 instances");
        }
        x[i] = p->roaring;
      }

      roaring_bitmap_t * r = op((TSize)arrayLength, x);
      gcaware_free(x);
      if (r == nullptr) {
        return v8utils::throwTypeError(isolate, opName, " failed roaring allocation");
      }

      self->replaceBitmapInstance(isolate, r);

      info.GetReturnValue().Set(result);

    } else {
      v8::Local<v8::Value> argv[] = {info[0]};
      auto vMaybe = cons->NewInstance(isolate->GetCurrentContext(), 1, argv);
      v8::Local<v8::Object> v;
      if (vMaybe.ToLocal(&v)) {
        info.GetReturnValue().Set(v);
      }
    }
  } else {
    v8::MaybeLocal<v8::Object> resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
    v8::Local<v8::Object> result;
    if (!resultMaybe.ToLocal(&result)) {
      return;
    }

    auto self = ObjectWrap::TryUnwrap<RoaringBitmap32>(result, isolate);
    if (!self) {
      return v8utils::throwError(isolate, ERROR_INVALID_OBJECT);
    }

    const auto ** x = (const roaring_bitmap_t **)gcaware_malloc(length * sizeof(roaring_bitmap_t *));
    if (x == nullptr) {
      return v8utils::throwTypeError(isolate, opName, " failed allocation");
    }

    for (int i = 0; i < length; ++i) {
      RoaringBitmap32 * p = ObjectWrap::TryUnwrap<RoaringBitmap32>(info, i);
      if (p == nullptr) {
        gcaware_free(x);
        return v8utils::throwTypeError(isolate, opName, " accepts only RoaringBitmap32 instances");
      }
      x[i] = p->roaring;
    }

    roaring_bitmap_t * r = op((TSize)length, x);
    gcaware_free(x);
    if (r == nullptr) {
      return v8utils::throwTypeError(isolate, opName, " failed roaring allocation");
    }

    self->replaceBitmapInstance(isolate, r);

    info.GetReturnValue().Set(result);
  }
}

void RoaringBitmap32_orManyStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  roaringOpMany("RoaringBitmap32::orMany", roaring_bitmap_or_many_heap, info);
}

void RoaringBitmap32_xorManyStatic(const v8::FunctionCallbackInfo<v8::Value> & info) {
  roaringOpMany("RoaringBitmap32::xorMany", roaring_bitmap_xor_many, info);
}

#endif  // ROARING_NODE_ROARINGBITMAP32_STATIC_OPS_
