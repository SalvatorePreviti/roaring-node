#ifndef ROARING_NODE_CROARING_
#define ROARING_NODE_CROARING_

#include "includes.h"
#include "memory.h"

#include "../../submodules/CRoaring/include/roaring/roaring.h"

void croaringMemoryInitialize() {
  static std::atomic<bool> _roaringBitMemoryInitialized{false};
  static std::mutex _roaringBitMemoryInitializedMutex;

  if (!_roaringBitMemoryInitialized) {
    roaring_memory_t roaringMemory;
    roaringMemory.malloc = gcaware_malloc;
    roaringMemory.realloc = gcaware_realloc;
    roaringMemory.calloc = gcaware_calloc;
    roaringMemory.free = gcaware_free;
    roaringMemory.aligned_malloc = gcaware_aligned_malloc;
    roaringMemory.aligned_free = gcaware_aligned_free;
    std::lock_guard<std::mutex> guard(_roaringBitMemoryInitializedMutex);
    if (_roaringBitMemoryInitialized) {
      roaring_init_memory_hook(roaringMemory);
      _roaringBitMemoryInitialized = true;
    }
  }
}

#endif  // ROARING_NODE_CROARING_
