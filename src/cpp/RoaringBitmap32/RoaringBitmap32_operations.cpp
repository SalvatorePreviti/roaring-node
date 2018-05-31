#include "../v8utils.h"

#include "../RoaringBitmap32Iterator/RoaringBitmap32Iterator.h"
#include "../TypedArrays.h"
#include "RoaringBitmap32.h"

void RoaringBitmap32::add(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 1 || !info[0]->IsUint32())
    return Nan::ThrowTypeError(Nan::New("RoaringBitmap32::add - 32 bit unsigned integer expected").ToLocalChecked());

  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  roaring_bitmap_add(&self->roaring, info[0]->Uint32Value());
  return info.GetReturnValue().Set(info.This());
}

void RoaringBitmap32::tryAdd(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 1 || !info[0]->IsUint32())
    return info.GetReturnValue().Set(false);

  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());

  auto oldSize = roaring_bitmap_get_cardinality(&self->roaring);
  roaring_bitmap_add(&self->roaring, info[0]->Uint32Value());
  return info.GetReturnValue().Set(oldSize != roaring_bitmap_get_cardinality(&self->roaring));
}

template <typename TArg>
void roaringAddMany(v8::Isolate * isolate, RoaringBitmap32 * self, const TArg & arg) {
  if (arg->IsUint32Array() || arg->IsInt32Array()) {
    Nan::TypedArrayContents<uint32_t> typedArray(arg);
    roaring_bitmap_add_many(&self->roaring, typedArray.length(), *typedArray);
    return;
  }

  if (arg->IsObject() && !arg->IsNull()) {
    if (RoaringBitmap32::constructorTemplate.Get(isolate)->HasInstance(arg)) {
      RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(arg->ToObject());
      roaring_bitmap_or_inplace(&self->roaring, &other->roaring);
    } else {
      v8::Local<v8::Value> argv[] = {arg};
      auto t = TypedArrays::Uint32Array_from.Get(isolate)->Call(TypedArrays::Uint32Array.Get(isolate), 1, argv);
      if (!t.IsEmpty()) {
        Nan::TypedArrayContents<uint32_t> typedArray(t);
        roaring_bitmap_add_many(&self->roaring, typedArray.length(), *typedArray);
      }
    }

    return;
  }

  return Nan::ThrowTypeError(Nan::New("Uint32Array, RoaringBitmap32 or Iterable<number> expected").ToLocalChecked());
}

void RoaringBitmap32::copyFrom(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() > 0) {
    auto const & arg = info[0];

    v8::Isolate * isolate = info.GetIsolate();
    RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());

    if (arg->IsUint32Array() || arg->IsInt32Array()) {
      Nan::TypedArrayContents<uint32_t> typedArray(arg);
      RoaringBitmap32::clear(info);
      roaring_bitmap_add_many(&self->roaring, typedArray.length(), *typedArray);
      return;
    }

    if (arg->IsObject() && !arg->IsNull()) {
      if (RoaringBitmap32::constructorTemplate.Get(isolate)->HasInstance(arg)) {
        RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(arg->ToObject());

        if (self == other) {
          return;
        }

        ra_clear(&self->roaring.high_low_container);
        bool is_ok = ra_copy(&other->roaring.high_low_container, &self->roaring.high_low_container, other->roaring.copy_on_write);
        if (!is_ok) {
          return Nan::ThrowError(Nan::New("RoaringBitmap32::copyFrom failed to copy").ToLocalChecked());
        }

        self->roaring.copy_on_write = other->roaring.copy_on_write;
      } else {
        v8::Local<v8::Value> argv[] = {arg};
        Nan::TypedArrayContents<uint32_t> typedArray(TypedArrays::Uint32Array_from.Get(isolate)->Call(TypedArrays::Uint32Array.Get(isolate), 1, argv));
        RoaringBitmap32::clear(info);
        roaring_bitmap_add_many(&self->roaring, typedArray.length(), *typedArray);
      }
    }
    return;
  }

  return Nan::ThrowTypeError(Nan::New("Uint32Array, RoaringBitmap32 or Iterable<number> expected").ToLocalChecked());
}

void RoaringBitmap32::addMany(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() > 0) {
    RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
    roaringAddMany(info.GetIsolate(), self, info[0]);
    return info.GetReturnValue().Set(info.This());
  }
  return Nan::ThrowTypeError(Nan::New("Uint32Array, RoaringBitmap32 or Iterable<number> expected").ToLocalChecked());
}

void RoaringBitmap32::removeMany(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() > 0) {
    auto const & arg = info[0];
    RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
    if (RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(arg)) {
      RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(arg->ToObject());
      roaring_bitmap_andnot_inplace(&self->roaring, &other->roaring);
      return info.GetReturnValue().Set(info.This());
    } else {
      RoaringBitmap32 tmp;
      roaringAddMany(info.GetIsolate(), &tmp, arg);
      roaring_bitmap_andnot_inplace(&self->roaring, &tmp.roaring);
      return info.GetReturnValue().Set(info.This());
    }
  }

  return Nan::ThrowTypeError(Nan::New("Uint32Array, RoaringBitmap32 or Iterable<number> expected").ToLocalChecked());
}

void RoaringBitmap32::andInPlace(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() > 0) {
    auto const & arg = info[0];
    RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
    if (RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(arg)) {
      RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(arg->ToObject());
      roaring_bitmap_and_inplace(&self->roaring, &other->roaring);
      return info.GetReturnValue().Set(info.This());
    } else {
      RoaringBitmap32 tmp;
      roaringAddMany(info.GetIsolate(), &tmp, arg);
      roaring_bitmap_and_inplace(&self->roaring, &tmp.roaring);
      return info.GetReturnValue().Set(info.This());
    }
  }

  return Nan::ThrowTypeError(Nan::New("Uint32Array, RoaringBitmap32 or Iterable<number> expected").ToLocalChecked());
}

void RoaringBitmap32::xorInPlace(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() > 0) {
    auto const & arg = info[0];
    RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
    if (RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(arg)) {
      RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(arg->ToObject());
      roaring_bitmap_xor_inplace(&self->roaring, &other->roaring);
      return info.GetReturnValue().Set(info.This());
    } else {
      RoaringBitmap32 tmp;
      roaringAddMany(info.GetIsolate(), &tmp, arg);
      roaring_bitmap_xor_inplace(&self->roaring, &tmp.roaring);
      return info.GetReturnValue().Set(info.This());
    }
  }

  return Nan::ThrowTypeError(Nan::New("Uint32Array, RoaringBitmap32 or Iterable<number> expected").ToLocalChecked());
}

void RoaringBitmap32::remove(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() >= 1 && info[0]->IsUint32()) {
    RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
    roaring_bitmap_remove(&self->roaring, info[0]->Uint32Value());
  }
}

void RoaringBitmap32::removeChecked(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 1 || !info[0]->IsUint32()) {
    info.GetReturnValue().Set(false);
  } else {
    RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
    auto oldSize = roaring_bitmap_get_cardinality(&self->roaring);
    if (oldSize == 0) {
      info.GetReturnValue().Set(false);
    } else {
      roaring_bitmap_remove(&self->roaring, info[0]->Uint32Value());
      info.GetReturnValue().Set(oldSize != roaring_bitmap_get_cardinality(&self->roaring));
    }
  }
}

void RoaringBitmap32::clear(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  roaring_bitmap_t newRoaring;
  if (!ra_init(&newRoaring.high_low_container)) {
    return Nan::ThrowError(Nan::New("RoaringBitmap32::clear - failed to initialize a new roaring container").ToLocalChecked());
  }
  ra_clear(&self->roaring.high_low_container);
  self->roaring.high_low_container = std::move(newRoaring.high_low_container);
}

void RoaringBitmap32::flipRange(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() >= 2 && info[0]->IsUint32() && info[1]->IsUint32()) {
    RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
    roaring_bitmap_flip_inplace(&self->roaring, info[0]->Uint32Value(), info[1]->Uint32Value());
  }
}

void RoaringBitmap32::swapStatic(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 2)
    return Nan::ThrowTypeError(Nan::New("RoaringBitmap32::swap expects 2 arguments").ToLocalChecked());
  if (!RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[0]))
    return Nan::ThrowTypeError(Nan::New("RoaringBitmap32::swap first argument must be a RoaringBitmap32").ToLocalChecked());
  if (!RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[1]))
    return Nan::ThrowTypeError(Nan::New("RoaringBitmap32::swap second argument must be a RoaringBitmap32").ToLocalChecked());
  RoaringBitmap32 * a = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  RoaringBitmap32 * b = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[1]->ToObject());

  if (a != b) {
    std::swap(a->roaring, b->roaring);
  }
}

void RoaringBitmap32::andStatic(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 2)
    return Nan::ThrowTypeError(Nan::New("RoaringBitmap32::and expects 2 arguments").ToLocalChecked());
  if (!RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[0]))
    return Nan::ThrowTypeError(Nan::New("RoaringBitmap32::and first argument must be a RoaringBitmap32").ToLocalChecked());
  if (!RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[1]))
    return Nan::ThrowTypeError(Nan::New("RoaringBitmap32::and second argument must be a RoaringBitmap32").ToLocalChecked());
  RoaringBitmap32 * a = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  RoaringBitmap32 * b = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[1]->ToObject());

  v8::Local<v8::Function> cons = Nan::New(constructor);
  v8::Local<v8::Value> argv[0] = {};

  auto resultMaybe = Nan::NewInstance(cons, 0, argv);
  if (resultMaybe.IsEmpty())
    return;

  auto result = resultMaybe.ToLocalChecked();
  auto self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(result);

  roaring_bitmap_t * r = roaring_bitmap_and(&a->roaring, &b->roaring);
  if (r == nullptr)
    return Nan::ThrowTypeError(Nan::New("RoaringBitmap32::and failed materalization").ToLocalChecked());

  ra_clear(&self->roaring.high_low_container);
  self->roaring.high_low_container = std::move(r->high_low_container);

  info.GetReturnValue().Set(result);
}

void RoaringBitmap32::orStatic(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 2)
    return Nan::ThrowTypeError(Nan::New("RoaringBitmap32::or expects 2 arguments").ToLocalChecked());
  if (!RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[0]))
    return Nan::ThrowTypeError(Nan::New("RoaringBitmap32::or first argument must be a RoaringBitmap32").ToLocalChecked());
  if (!RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[1]))
    return Nan::ThrowTypeError(Nan::New("RoaringBitmap32::or second argument must be a RoaringBitmap32").ToLocalChecked());

  RoaringBitmap32 * a = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  RoaringBitmap32 * b = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[1]->ToObject());

  v8::Local<v8::Function> cons = Nan::New(constructor);
  v8::Local<v8::Value> argv[0] = {};

  auto resultMaybe = Nan::NewInstance(cons, 0, argv);
  if (resultMaybe.IsEmpty())
    return;

  auto result = resultMaybe.ToLocalChecked();
  auto self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(result);

  roaring_bitmap_t * r = roaring_bitmap_or(&a->roaring, &b->roaring);
  if (r == nullptr)
    return Nan::ThrowTypeError(Nan::New("RoaringBitmap32::or failed materalization").ToLocalChecked());

  ra_clear(&self->roaring.high_low_container);
  self->roaring.high_low_container = std::move(r->high_low_container);

  info.GetReturnValue().Set(result);
}

void RoaringBitmap32::xorStatic(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 2)
    return Nan::ThrowTypeError(Nan::New("RoaringBitmap32::xor expects 2 arguments").ToLocalChecked());
  if (!RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[0]))
    return Nan::ThrowTypeError(Nan::New("RoaringBitmap32::xor first argument must be a RoaringBitmap32").ToLocalChecked());
  if (!RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[1]))
    return Nan::ThrowTypeError(Nan::New("RoaringBitmap32::xor second argument must be a RoaringBitmap32").ToLocalChecked());

  RoaringBitmap32 * a = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  RoaringBitmap32 * b = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[1]->ToObject());

  v8::Local<v8::Function> cons = Nan::New(constructor);
  v8::Local<v8::Value> argv[0] = {};

  auto resultMaybe = Nan::NewInstance(cons, 0, argv);
  if (resultMaybe.IsEmpty())
    return;

  auto result = resultMaybe.ToLocalChecked();
  auto self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(result);

  roaring_bitmap_t * r = roaring_bitmap_xor(&a->roaring, &b->roaring);
  if (r == nullptr)
    return Nan::ThrowTypeError(Nan::New("RoaringBitmap32::xor failed materalization").ToLocalChecked());

  ra_clear(&self->roaring.high_low_container);
  self->roaring.high_low_container = std::move(r->high_low_container);

  info.GetReturnValue().Set(result);
}

void RoaringBitmap32::andNotStatic(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 2)
    return Nan::ThrowTypeError(Nan::New("RoaringBitmap32::andnot expects 2 arguments").ToLocalChecked());
  if (!RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[0]))
    return Nan::ThrowTypeError(Nan::New("RoaringBitmap32::andnot first argument must be a RoaringBitmap32").ToLocalChecked());
  if (!RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[1]))
    return Nan::ThrowTypeError(Nan::New("RoaringBitmap32::andnot second argument must be a RoaringBitmap32").ToLocalChecked());

  RoaringBitmap32 * a = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  RoaringBitmap32 * b = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[1]->ToObject());

  v8::Local<v8::Function> cons = Nan::New(constructor);
  v8::Local<v8::Value> argv[0] = {};

  auto resultMaybe = Nan::NewInstance(cons, 0, argv);
  if (resultMaybe.IsEmpty())
    return;

  auto result = resultMaybe.ToLocalChecked();
  auto self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(result);

  roaring_bitmap_t * r = roaring_bitmap_andnot(&a->roaring, &b->roaring);
  if (r == nullptr)
    return Nan::ThrowTypeError(Nan::New("RoaringBitmap32::andnot failed materalization").ToLocalChecked());

  ra_clear(&self->roaring.high_low_container);
  self->roaring.high_low_container = std::move(r->high_low_container);

  info.GetReturnValue().Set(result);
}