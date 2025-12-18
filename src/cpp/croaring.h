#ifndef ROARING_NODE_CROARING_
#define ROARING_NODE_CROARING_

#include "includes.h"
#include "memory.h"

#include "../../submodules/CRoaring/include/roaring/roaring.h"

void croaringMemoryInitialize() {
  static std::atomic<bool> roaringMemoryInitialized{false};
  static std::mutex roaringMemoryInitializedMutex;

  if (roaringMemoryInitialized.load(std::memory_order_acquire)) {
    return;
  }

  std::lock_guard<std::mutex> guard(roaringMemoryInitializedMutex);
  if (roaringMemoryInitialized.load(std::memory_order_relaxed)) {
    return;
  }

  roaring_memory_t roaringMemory{};
  roaringMemory.malloc = gcaware_malloc;
  roaringMemory.realloc = gcaware_realloc;
  roaringMemory.calloc = gcaware_calloc;
  roaringMemory.free = gcaware_free;
  roaringMemory.aligned_malloc = gcaware_aligned_malloc;
  roaringMemory.aligned_free = gcaware_aligned_free;

  roaring_init_memory_hook(roaringMemory);
  roaringMemoryInitialized.store(true, std::memory_order_release);
}

#endif  // ROARING_NODE_CROARING_
