#ifndef __ROARINGBITMAP32__H__
#define __ROARINGBITMAP32__H__

#include "v8utils/v8utils.h"
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
  frozen_croaring = 2,
};

struct DeserializeResult final {
  roaring_bitmap_t_ptr bitmap;
  const char * error;

  inline explicit DeserializeResult(roaring_bitmap_t_ptr bitmap, const char * error = nullptr) :
    bitmap(bitmap),
    error(
      bitmap != nullptr
        ? nullptr
        : (error != nullptr ? error : "RoaringBitmap32::deserialize - failed to deserialize roaring bitmap")) {}
};

enum class FrozenMode { Unfrozen = 0, SoftFrozen = 1, HardFrozen = 2 };

enum class SerializationMode { croaring = 0, portable = 1, frozen = 2 };

enum class DeserializationMode { croaring = 0, portable = 1, frozen = 2, frozen_portable = 3 };

class RoaringBitmap32 final {
 public:
  static constexpr const uint64_t OBJECT_TOKEN = 0x21524F4152333221;

  static const uint64_t objectToken = OBJECT_TOKEN;

  roaring_bitmap_t * roaring;
  uint64_t version;
  int64_t amountOfExternalAllocatedMemoryTracker;
  v8::Persistent<v8::Object> persistent;
  FrozenMode frozenMode;

  inline void invalidate() { ++version; }

  void updateAmountOfExternalAllocatedMemory(v8::Isolate * isolate);
  void updateAmountOfExternalAllocatedMemory(v8::Isolate * isolate, size_t newSize);

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

  static void freeze(const v8::FunctionCallbackInfo<v8::Value> & info);

  static void isEmpty_getter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> & info);
  static void size_getter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> & info);
  static void isFrozen_getter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> & info);

  explicit RoaringBitmap32(uint32_t capacity);
  ~RoaringBitmap32();

 private:
  static void WeakCallback(v8::WeakCallbackInfo<RoaringBitmap32> const & info);
  static DeserializeResult doDeserialize(const v8utils::TypedArrayContent<uint8_t> & typedArray, bool portable);
  static SerializationFormat tryParseSerializationFormat(
    const v8::MaybeLocal<v8::Value> & maybeValue, v8::Isolate * isolate);
  static SerializationFormat tryParseSerializationFormat(const v8::Local<v8::Value> & value, v8::Isolate * isolate);

  friend class DeserializeWorker;
  friend class DeserializeParallelWorker;
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

  v8::Persistent<v8::Object> persistent;
  roaring_uint32_iterator_t it;
  uint64_t bitmapVersion;
  RoaringBitmap32 * bitmapInstance;
  v8utils::TypedArrayContent<uint32_t> bufferContent;

  v8::Persistent<v8::Object> buffer;
  v8::Persistent<v8::Object> bitmap;

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
