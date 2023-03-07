#ifndef __ROARINGBITMAP32__H__
#define __ROARINGBITMAP32__H__

#include "v8utils.h"
#include "CRoaringUnityBuild/roaring.h"
#include "serialization-format.h"

using namespace roaring;
using namespace roaring::api;

class RoaringBitmap32;

typedef roaring_bitmap_t * roaring_bitmap_t_ptr;

class RoaringBitmap32 final : public ObjectWrap {
 public:
  static const constexpr uint64_t OBJECT_TOKEN = 0x21524F4152333221;

  static const constexpr int64_t FROZEN_COUNTER_SOFT_FROZEN = -1;
  static const constexpr int64_t FROZEN_COUNTER_HARD_FROZEN = -2;

  roaring_bitmap_t * roaring;
  int64_t sizeCache;
  int64_t _version;
  int64_t frozenCounter;
  RoaringBitmap32 * const readonlyViewOf;
  v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> readonlyViewPersistent;
  v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> persistent;
  v8utils::TypedArrayContent<uint8_t> frozenStorage;

  inline bool isEmpty() const {
    if (this->sizeCache == 0) {
      return true;
    }
    if (this->readonlyViewOf != nullptr) {
      return this->readonlyViewOf->isEmpty();
    }
    const roaring_bitmap_t_ptr roaring = this->roaring;
    bool result = roaring == nullptr || roaring_bitmap_is_empty(roaring);
    if (result) {
      const_cast<RoaringBitmap32 *>(this)->sizeCache = 0;
    }
    return result;
  }

  inline size_t getSize() const {
    int64_t size = this->sizeCache;
    if (size < 0) {
      if (this->readonlyViewOf != nullptr) {
        return this->readonlyViewOf->getSize();
      }
      const roaring_bitmap_t_ptr roaring = this->roaring;
      size = roaring != nullptr ? (int64_t)roaring_bitmap_get_cardinality(roaring) : 0;
      const_cast<RoaringBitmap32 *>(this)->sizeCache = size;
    }
    return (size_t)size;
  }

  inline bool isFrozen() const { return this->frozenCounter != 0; }
  inline bool isFrozenHard() const { return this->frozenCounter > 0 || this->frozenCounter == FROZEN_COUNTER_HARD_FROZEN; }
  inline bool isFrozenForever() const { return this->frozenCounter < 0; }

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

  inline int64_t getVersion() const { return this->_version; }

  inline void invalidate() {
    this->sizeCache = -1;
    ++this->_version;
  }

  bool replaceBitmapInstance(v8::Isolate * isolate, roaring_bitmap_t * newInstance) {
    roaring_bitmap_t * oldInstance = this->roaring;
    if (oldInstance == newInstance) {
      return false;
    }
    if (oldInstance != nullptr) {
      roaring_bitmap_free(oldInstance);
    }
    if (newInstance != nullptr && is_frozen(newInstance)) {
      this->frozenCounter = RoaringBitmap32::FROZEN_COUNTER_HARD_FROZEN;
    }
    this->roaring = newInstance;
    this->invalidate();
    return true;
  }

  static void asReadonlyView(const v8::FunctionCallbackInfo<v8::Value> & info);

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
  static void intersectsWithRange(const v8::FunctionCallbackInfo<v8::Value> & info);
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

  static void fromRangeStatic(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void fromArrayStaticAsync(const v8::FunctionCallbackInfo<v8::Value> & info);

  static void addOffsetStatic(const v8::FunctionCallbackInfo<v8::Value> & info);
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

  static void getInstanceCountStatic(const v8::FunctionCallbackInfo<v8::Value> & info);

  explicit RoaringBitmap32(AddonData * addonData, RoaringBitmap32 * readonlyViewOf) :
    ObjectWrap(addonData, RoaringBitmap32::OBJECT_TOKEN),
    roaring(readonlyViewOf->roaring),
    sizeCache(-1),
    frozenCounter(RoaringBitmap32::FROZEN_COUNTER_HARD_FROZEN),
    readonlyViewOf(readonlyViewOf->readonlyViewOf ? readonlyViewOf->readonlyViewOf : readonlyViewOf) {
    gcaware_addAllocatedMemory(sizeof(RoaringBitmap32));
  }

  explicit RoaringBitmap32(AddonData * addonData, uint32_t capacity) :
    ObjectWrap(addonData, RoaringBitmap32::OBJECT_TOKEN),
    roaring(roaring_bitmap_create_with_capacity(capacity)),
    sizeCache(0),
    _version(0),
    frozenCounter(0),
    readonlyViewOf(nullptr) {
    ++addonData->RoaringBitmap32_instances;
    gcaware_addAllocatedMemory(sizeof(RoaringBitmap32));
  }

  ~RoaringBitmap32() {
    gcaware_removeAllocatedMemory(sizeof(RoaringBitmap32));
    if (!this->readonlyViewOf) {
      --this->addonData->RoaringBitmap32_instances;
      if (this->roaring != nullptr) {
        roaring_bitmap_free(this->roaring);
      }
      if (this->frozenStorage.data != nullptr && this->frozenStorage.length == std::numeric_limits<size_t>::max()) {
        gcaware_aligned_free(this->frozenStorage.data);
      }
    }
    if (!this->persistent.IsEmpty()) {
      this->persistent.ClearWeak();
    }
  }
};

static void RoaringBitmap32_WeakCallback(v8::WeakCallbackInfo<RoaringBitmap32> const & info) {
  RoaringBitmap32 * p = info.GetParameter();
  if (p != nullptr) {
    delete p;
  }
}

#endif
