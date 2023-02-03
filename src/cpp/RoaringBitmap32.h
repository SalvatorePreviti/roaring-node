#ifndef __ROARINGBITMAP32__H__
#define __ROARINGBITMAP32__H__

#include "v8utils.h"
#include "CRoaringUnityBuild/roaring_version_string.h"
#include "CRoaringUnityBuild/roaring.h"

using namespace roaring;
using namespace roaring::api;

class RoaringBitmap32;

typedef roaring_bitmap_t * roaring_bitmap_t_ptr;

enum class SerializationFormat {
  INVALID = -1,
  croaring = 0,
  portable = 1,
  unsafe_frozen_croaring = 2,
};

enum class DeserializationFormat {
  INVALID = -1,
  croaring = 0,
  portable = 1,
  unsafe_frozen_croaring = 2,
  unsafe_frozen_portable = 3,
};

enum class FrozenViewFormat {
  INVALID = -1,
  unsafe_frozen_croaring = 2,
  unsafe_frozen_portable = 3,
};

class RoaringBitmap32 final {
 public:
  static const constexpr uint64_t OBJECT_TOKEN = 0x21524F4152333221;

  static const constexpr int64_t FROZEN_COUNTER_SOFT_FROZEN = -1;
  static const constexpr int64_t FROZEN_COUNTER_HARD_FROZEN = -2;

  static const uint64_t objectToken = OBJECT_TOKEN;

  roaring_bitmap_t * roaring;
  uint64_t version;
  int64_t frozenCounter;
  v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> persistent;
  v8utils::TypedArrayContent<uint8_t> frozenStorage;

  inline bool isFrozen() const { return this->frozenCounter != 0; }
  inline bool isFrozenHard() const { return this->frozenCounter > 0 || this->frozenCounter == FROZEN_COUNTER_HARD_FROZEN; }

  inline void beginFreeze() {
    if (this->frozenCounter >= 0) {
      ++this->frozenCounter;
    }
  }

  inline void endFreeze() {
    if (this->frozenCounter > 0) {
      --this->frozenCounter;
    }
  }

  inline void invalidate() { ++this->version; }

  bool replaceBitmapInstance(v8::Isolate * isolate, roaring_bitmap_t * newInstance);

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
  static void toUint32ArrayAsync(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void rangeUint32Array(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void toArray(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void toSet(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void getSerializationSizeInBytes(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void serialize(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void serializeAsync(const v8::FunctionCallbackInfo<v8::Value> & info);

  static void deserialize(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void deserializeStatic(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void deserializeStaticAsync(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void deserializeParallelStaticAsync(const v8::FunctionCallbackInfo<v8::Value> & info);

  static void unsafeFrozenViewStatic(const v8::FunctionCallbackInfo<v8::Value> & info);

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

  static void freeze(const v8::FunctionCallbackInfo<v8::Value> & info);

  static void isEmpty_getter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> & info);
  static void size_getter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> & info);
  static void isFrozen_getter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> & info);

  explicit RoaringBitmap32(uint32_t capacity);
  ~RoaringBitmap32();

  static void WeakCallback(v8::WeakCallbackInfo<RoaringBitmap32> const & info);
};

class RoaringBitmap32FactoryAsyncWorker : public v8utils::AsyncWorker {
 public:
  volatile roaring_bitmap_t_ptr bitmap;

  explicit RoaringBitmap32FactoryAsyncWorker(v8::Isolate * isolate);
  virtual ~RoaringBitmap32FactoryAsyncWorker();

 protected:
  v8::Local<v8::Value> done() override;
};

class RoaringBitmap32BufferedIterator {
 public:
  static constexpr const uint64_t OBJECT_TOKEN = 0x21524F4152495421;

  static const uint64_t objectToken = OBJECT_TOKEN;

  enum { allocatedMemoryDelta = 1024 };

  v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> persistent;
  roaring_uint32_iterator_t it;
  uint64_t bitmapVersion;
  RoaringBitmap32 * bitmapInstance;
  v8utils::TypedArrayContent<uint32_t> bufferContent;

  v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> bitmap;

  static v8::Eternal<v8::FunctionTemplate> constructorTemplate;
  static v8::Eternal<v8::Function> constructor;

  static void Init(v8::Local<v8::Object> exports);
  static void New(const v8::FunctionCallbackInfo<v8::Value> & info);

  static void fill(const v8::FunctionCallbackInfo<v8::Value> & info);

  RoaringBitmap32BufferedIterator();
  ~RoaringBitmap32BufferedIterator();

 private:
  void destroy();
  static v8::Eternal<v8::String> nPropertyName;
  static void WeakCallback(v8::WeakCallbackInfo<RoaringBitmap32BufferedIterator> const & info);
};

#endif
