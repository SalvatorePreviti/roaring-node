#include "../v8utils.h"

#include "RoaringBitmap32.h"

void RoaringBitmap32::isSubset(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 1 || !RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[0]))
    return info.GetReturnValue().Set(false);
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  info.GetReturnValue().Set(self == other || roaring_bitmap_is_subset(&self->roaring, &other->roaring));
}

void RoaringBitmap32::isStrictSubset(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 1 || !RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[0]))
    return info.GetReturnValue().Set(false);
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  info.GetReturnValue().Set(roaring_bitmap_is_strict_subset(&self->roaring, &other->roaring));
}

void RoaringBitmap32::isEqual(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 1 || !RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[0]))
    return info.GetReturnValue().Set(false);
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  info.GetReturnValue().Set(self == other || roaring_bitmap_equals(&self->roaring, &other->roaring));
}

void RoaringBitmap32::intersects(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 1 || !RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[0]))
    return info.GetReturnValue().Set(false);
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  info.GetReturnValue().Set(roaring_bitmap_intersect(&self->roaring, &other->roaring));
}

void RoaringBitmap32::andCardinality(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 1 || !RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[0]))
    return info.GetReturnValue().Set(-1);
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  info.GetReturnValue().Set((double)roaring_bitmap_and_cardinality(&self->roaring, &other->roaring));
}

void RoaringBitmap32::orCardinality(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 1 || !RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[0]))
    return info.GetReturnValue().Set(-1);
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  info.GetReturnValue().Set((double)roaring_bitmap_or_cardinality(&self->roaring, &other->roaring));
}

void RoaringBitmap32::andNotCardinality(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 1 || !RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[0]))
    return info.GetReturnValue().Set(-1);
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  info.GetReturnValue().Set((double)roaring_bitmap_andnot_cardinality(&self->roaring, &other->roaring));
}

void RoaringBitmap32::xorCardinality(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 1 || !RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[0]))
    return info.GetReturnValue().Set(-1);
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  info.GetReturnValue().Set((double)roaring_bitmap_xor_cardinality(&self->roaring, &other->roaring));
}

void RoaringBitmap32::jaccardIndex(const Nan::FunctionCallbackInfo<v8::Value> & info) {
  if (info.Length() < 1 || !RoaringBitmap32::constructorTemplate.Get(info.GetIsolate())->HasInstance(info[0]))
    return info.GetReturnValue().Set(-1);
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  info.GetReturnValue().Set(roaring_bitmap_jaccard_index(&self->roaring, &other->roaring));
}