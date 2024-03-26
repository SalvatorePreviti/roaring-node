#ifndef __ROARINGBITMAP32__H__
#define __ROARINGBITMAP32__H__

#include "v8utils.h"
#include "serialization-format.h"
#include "WorkerError.h"

using namespace roaring;
using namespace roaring::api;

class RoaringBitmap32;

typedef roaring_bitmap_t * roaring_bitmap_t_ptr;

std::atomic<uint64_t> RoaringBitmap32_instances;

class RoaringBitmap32 final {
 public:
  static const constexpr uint64_t OBJECT_TOKEN = 0x21524F4152330000;

  static const constexpr int64_t FROZEN_COUNTER_SOFT_FROZEN = -1;
  static const constexpr int64_t FROZEN_COUNTER_HARD_FROZEN = -2;

  roaring_bitmap_t * roaring;
  AddonData * const addonData;
  int64_t sizeCache;
  int64_t _version;
  int64_t frozenCounter;
  RoaringBitmap32 * const readonlyViewOf;
  v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> readonlyViewPersistent;
  v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> persistent;
  v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> addonDataPersistent;
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

  inline bool roaring_bitmap_t_is_frozen(const roaring_bitmap_t * r) {
    return r->high_low_container.flags & ROARING_FLAG_FROZEN;
  }

  bool replaceBitmapInstance(v8::Isolate * isolate, roaring_bitmap_t * newInstance) {
    roaring_bitmap_t * oldInstance = this->roaring;
    if (oldInstance == newInstance) {
      return false;
    }
    if (oldInstance != nullptr) {
      roaring_bitmap_free(oldInstance);
    }
    if (newInstance != nullptr && roaring_bitmap_t_is_frozen(newInstance)) {
      this->frozenCounter = RoaringBitmap32::FROZEN_COUNTER_HARD_FROZEN;
    }
    this->roaring = newInstance;
    this->invalidate();
    return true;
  }

  explicit RoaringBitmap32(AddonData * addonData, RoaringBitmap32 * readonlyViewOf) :
    roaring(readonlyViewOf->roaring),
    addonData(addonData),
    sizeCache(-1),
    frozenCounter(RoaringBitmap32::FROZEN_COUNTER_HARD_FROZEN),
    readonlyViewOf(readonlyViewOf->readonlyViewOf ? readonlyViewOf->readonlyViewOf : readonlyViewOf) {
    gcaware_addAllocatedMemory(sizeof(RoaringBitmap32));
  }

  explicit RoaringBitmap32(AddonData * addonData, uint32_t capacity) :
    roaring(roaring_bitmap_create_with_capacity(capacity)),
    addonData(addonData),
    sizeCache(0),
    _version(0),
    frozenCounter(0),
    readonlyViewOf(nullptr) {
    ++RoaringBitmap32_instances;
    gcaware_addAllocatedMemory(sizeof(RoaringBitmap32));
  }

  ~RoaringBitmap32() {
    this->readonlyViewPersistent.Reset();
    if (!this->persistent.IsEmpty()) {
      this->persistent.ClearWeak();
      this->persistent.Reset();
    }
    gcaware_removeAllocatedMemory(sizeof(RoaringBitmap32));
    if (!this->readonlyViewOf) {
      --RoaringBitmap32_instances;
      if (this->roaring != nullptr) {
        roaring_bitmap_free(this->roaring);
      }
      if (this->frozenStorage.data != nullptr && this->frozenStorage.length == std::numeric_limits<size_t>::max()) {
        gcaware_aligned_free(this->frozenStorage.data);
      }
    }
  }
};

#endif
