#ifndef __ROARINGBITMAP32__H__
#define __ROARINGBITMAP32__H__

#include <cmath>
#include "../roaring.h"
#include "../v8utils/v8utils.h"

class RoaringBitmap32;

class RoaringBitmap32 : public v8utils::ObjectWrap {
 public:
  roaring_bitmap_t roaring;

  static v8::Persistent<v8::FunctionTemplate> constructorTemplate;
  static v8::Persistent<v8::Function> constructor;

  static void Init(v8::Local<v8::Object> exports);

  static void New(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void has(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void hasRange(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void copyFrom(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void add(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void tryAdd(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void addMany(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void andInPlace(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void xorInPlace(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void remove(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void removeMany(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void removeChecked(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void clear(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void minimum(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void maximum(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void isSubset(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void isStrictSubset(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void intersects(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void isEqual(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void andCardinality(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void orCardinality(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void andNotCardinality(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void xorCardinality(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void jaccardIndex(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void flipRange(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void addRange(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void rank(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void select(const v8::FunctionCallbackInfo<v8::Value> & info);

  static void removeRunCompression(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void runOptimize(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void shrinkToFit(const v8::FunctionCallbackInfo<v8::Value> & info);

  static void toUint32Array(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void toArray(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void toSet(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void getSerializationSizeInBytes(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void serialize(const v8::FunctionCallbackInfo<v8::Value> & info);

  static void deserialize(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void deserializeStatic(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void deserializeStaticAsync(const v8::FunctionCallbackInfo<v8::Value> & info);

  static void fromArrayStaticAsync(const v8::FunctionCallbackInfo<v8::Value> & info);

  static void andStatic(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void orStatic(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void xorStatic(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void andNotStatic(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void orManyStatic(const v8::FunctionCallbackInfo<v8::Value> & info);

  static void swapStatic(const v8::FunctionCallbackInfo<v8::Value> & info);

  static void clone(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void toString(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void contentToString(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void statistics(const v8::FunctionCallbackInfo<v8::Value> & info);

  static void isEmpty_getter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> & info);
  static void size_getter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> & info);

  RoaringBitmap32();

  virtual ~RoaringBitmap32();

  static const uint8_t roaring_bitmap_zero[sizeof(roaring_bitmap_t)];

 private:
  static void deserializeInner(const v8::FunctionCallbackInfo<v8::Value> & info, bool isStatic);
  static const char * doDeserialize(const v8utils::TypedArrayContent<uint8_t> & typedArray, bool portable, roaring_bitmap_t & newRoaring);

  friend struct DeserializeWorker;
};

class RoaringBitmap32FactoryAsyncWorker : public v8utils::AsyncWorker {
 public:
  roaring_bitmap_t bitmap;

  RoaringBitmap32FactoryAsyncWorker(v8::Isolate * isolate) :
      v8utils::AsyncWorker(isolate),
      bitmap(*((roaring_bitmap_t *)&RoaringBitmap32::roaring_bitmap_zero)) {
  }

 protected:
  virtual v8::Local<v8::Value> done() {
    v8::Local<v8::Function> cons = RoaringBitmap32::constructor.Get(isolate);

    v8::MaybeLocal<v8::Object> resultMaybe = cons->NewInstance(isolate->GetCurrentContext(), 0, nullptr);
    if (resultMaybe.IsEmpty()) {
      ra_clear(&bitmap.high_low_container);
      return empty();
    }

    v8::Local<v8::Object> result = resultMaybe.ToLocalChecked();

    RoaringBitmap32 * unwrapped = v8utils::ObjectWrap::Unwrap<RoaringBitmap32>(result);

    unwrapped->roaring = std::move(bitmap);

    return result;
  }
};

#endif