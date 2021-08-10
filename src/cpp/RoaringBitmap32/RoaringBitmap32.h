#ifndef __ROARINGBITMAP32__H__
#define __ROARINGBITMAP32__H__

#include <cmath>
#include "../roaring.h"
#include "../v8utils/v8utils.h"

class RoaringBitmap32;

class RoaringBitmap32 : NonCopyable {
 public:
  roaring_bitmap_t * roaring;
  uint64_t version;
  v8::Persistent<v8::Object> persistent;

  inline void invalidate() {
    ++version;
  }

  inline bool replaceBitmapInstance(roaring_bitmap_t * newInstance) {
    if (this->roaring != newInstance) {
      if (this->roaring) {
        roaring_bitmap_free(this->roaring);
        this->roaring = nullptr;
      }
      this->roaring = newInstance;
      this->invalidate();
      return true;
    }
    return false;
  }

  static v8::Eternal<v8::FunctionTemplate> constructorTemplate;
  static v8::Eternal<v8::Function> constructor;

  static void Init(v8::Local<v8::Object> exports);

  static void New(const v8::FunctionCallbackInfo<v8::Value> & info);

  static void fromRangeStatic(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void hasRange(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void flipRange(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void addRange(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void removeRange(const v8::FunctionCallbackInfo<v8::Value> & info);

  static void has(const v8::FunctionCallbackInfo<v8::Value> & info);
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
  static void rank(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void select(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void rangeCardinality(const v8::FunctionCallbackInfo<v8::Value> & info);

  static void removeRunCompression(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void runOptimize(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void shrinkToFit(const v8::FunctionCallbackInfo<v8::Value> & info);

  static void toUint32Array(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void rangeUint32Array(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void toArray(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void toSet(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void getSerializationSizeInBytes(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void serialize(const v8::FunctionCallbackInfo<v8::Value> & info);

  static void deserialize(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void deserializeStatic(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void deserializeStaticAsync(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void deserializeParallelStaticAsync(const v8::FunctionCallbackInfo<v8::Value> & info);

  static void fromArrayStaticAsync(const v8::FunctionCallbackInfo<v8::Value> & info);

  static void andStatic(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void orStatic(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void xorStatic(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void andNotStatic(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void orManyStatic(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void xorManyStatic(const v8::FunctionCallbackInfo<v8::Value> & info);

  static void swapStatic(const v8::FunctionCallbackInfo<v8::Value> & info);

  static void clone(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void toString(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void contentToString(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void statistics(const v8::FunctionCallbackInfo<v8::Value> & info);

  static void isEmpty_getter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> & info);
  static void size_getter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> & info);

  RoaringBitmap32(uint32_t capacity);
  ~RoaringBitmap32();

 private:
  void destroy();
  static void WeakCallback(v8::WeakCallbackInfo<RoaringBitmap32> const & info);
  static const char * doDeserialize(const v8utils::TypedArrayContent<uint8_t> & typedArray, bool portable, roaring_bitmap_t ** newRoaring);

  friend class DeserializeWorker;
  friend class DeserializeParallelWorker;
};

class RoaringBitmap32FactoryAsyncWorker : public v8utils::AsyncWorker {
 public:
  roaring_bitmap_t * bitmap;

  RoaringBitmap32FactoryAsyncWorker(v8::Isolate * isolate);
  virtual ~RoaringBitmap32FactoryAsyncWorker();

 protected:
  virtual v8::Local<v8::Value> done();
};

#endif
