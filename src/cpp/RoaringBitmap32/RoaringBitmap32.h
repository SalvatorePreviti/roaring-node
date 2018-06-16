#ifndef __ROARINGBITMAP32__H__
#define __ROARINGBITMAP32__H__

#include <nan.h>
#include <cmath>
#include "../roaring.h"

class RoaringBitmap32;
class RoaringBitmap32Iterator;

class RoaringBitmap32 : public Nan::ObjectWrap {
 public:
  roaring_bitmap_t roaring;

  static Nan::Persistent<v8::FunctionTemplate> constructorTemplate;
  static Nan::Persistent<v8::Function> constructor;

  static void Init(v8::Local<v8::Object> exports);

  static NAN_PROPERTY_GETTER(namedPropertyGetter);
  static NAN_PROPERTY_GETTER(isEmpty_getter);
  static NAN_PROPERTY_GETTER(size_getter);

  static void New(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void has(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void hasRange(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void copyFrom(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void add(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void tryAdd(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void addMany(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void andInPlace(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void xorInPlace(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void remove(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void removeMany(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void removeChecked(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void clear(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void minimum(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void maximum(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void isSubset(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void isStrictSubset(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void intersects(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void isEqual(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void andCardinality(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void orCardinality(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void andNotCardinality(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void xorCardinality(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void jaccardIndex(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void flipRange(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void addRange(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void rank(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void select(const Nan::FunctionCallbackInfo<v8::Value> & info);

  static void removeRunCompression(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void runOptimize(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void shrinkToFit(const Nan::FunctionCallbackInfo<v8::Value> & info);

  static void toUint32Array(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void toArray(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void toSet(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void getSerializationSizeInBytes(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void serialize(const Nan::FunctionCallbackInfo<v8::Value> & info);

  static void deserialize(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void deserializeStatic(const Nan::FunctionCallbackInfo<v8::Value> & info);

  static void swapStatic(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void andStatic(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void orStatic(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void xorStatic(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void andNotStatic(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void orManyStatic(const Nan::FunctionCallbackInfo<v8::Value> & info);

  static void clone(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void toString(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void contentToString(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void statistics(const Nan::FunctionCallbackInfo<v8::Value> & info);

  RoaringBitmap32();

  virtual ~RoaringBitmap32();

 private:
  static void deserializeInner(const Nan::FunctionCallbackInfo<v8::Value> & info, bool isStatic);
};

#endif