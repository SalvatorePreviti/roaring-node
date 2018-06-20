#include "../v8utils.h"

#include "RoaringBitmap32.h"

void RoaringBitmap32::isSubset(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  if (info.Length() < 1 || !RoaringBitmap32::constructorTemplate.Get(isolate)->HasInstance(info[0]))
    return info.GetReturnValue().Set(false);
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  info.GetReturnValue().Set(self == other || roaring_bitmap_is_subset(&self->roaring, &other->roaring));
}

void RoaringBitmap32::isStrictSubset(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  if (info.Length() < 1 || !RoaringBitmap32::constructorTemplate.Get(isolate)->HasInstance(info[0]))
    return info.GetReturnValue().Set(false);
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  info.GetReturnValue().Set(roaring_bitmap_is_strict_subset(&self->roaring, &other->roaring));
}

void RoaringBitmap32::isEqual(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  if (info.Length() < 1 || !RoaringBitmap32::constructorTemplate.Get(isolate)->HasInstance(info[0]))
    return info.GetReturnValue().Set(false);
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  info.GetReturnValue().Set(self == other || roaring_bitmap_equals(&self->roaring, &other->roaring));
}

void RoaringBitmap32::intersects(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  if (info.Length() < 1 || !RoaringBitmap32::constructorTemplate.Get(isolate)->HasInstance(info[0]))
    return info.GetReturnValue().Set(false);
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  info.GetReturnValue().Set(roaring_bitmap_intersect(&self->roaring, &other->roaring));
}

void RoaringBitmap32::andCardinality(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  if (info.Length() < 1 || !RoaringBitmap32::constructorTemplate.Get(isolate)->HasInstance(info[0]))
    return info.GetReturnValue().Set(-1);
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  info.GetReturnValue().Set((double)roaring_bitmap_and_cardinality(&self->roaring, &other->roaring));
}

void RoaringBitmap32::orCardinality(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  if (info.Length() < 1 || !RoaringBitmap32::constructorTemplate.Get(isolate)->HasInstance(info[0]))
    return info.GetReturnValue().Set(-1);
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  info.GetReturnValue().Set((double)roaring_bitmap_or_cardinality(&self->roaring, &other->roaring));
}

void RoaringBitmap32::andNotCardinality(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  if (info.Length() < 1 || !RoaringBitmap32::constructorTemplate.Get(isolate)->HasInstance(info[0]))
    return info.GetReturnValue().Set(-1);
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  info.GetReturnValue().Set((double)roaring_bitmap_andnot_cardinality(&self->roaring, &other->roaring));
}

void RoaringBitmap32::xorCardinality(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  if (info.Length() < 1 || !RoaringBitmap32::constructorTemplate.Get(isolate)->HasInstance(info[0]))
    return info.GetReturnValue().Set(-1);
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  info.GetReturnValue().Set((double)roaring_bitmap_xor_cardinality(&self->roaring, &other->roaring));
}

void RoaringBitmap32::jaccardIndex(const v8::FunctionCallbackInfo<v8::Value> & info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);

  if (info.Length() < 1 || !RoaringBitmap32::constructorTemplate.Get(isolate)->HasInstance(info[0]))
    return info.GetReturnValue().Set(-1);
  RoaringBitmap32 * self = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info.Holder());
  RoaringBitmap32 * other = Nan::ObjectWrap::Unwrap<RoaringBitmap32>(info[0]->ToObject());
  info.GetReturnValue().Set(roaring_bitmap_jaccard_index(&self->roaring, &other->roaring));
}