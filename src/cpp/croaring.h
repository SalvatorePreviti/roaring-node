#ifndef ROARING_NODE_CROARING_
#define ROARING_NODE_CROARING_

#include "includes.h"
#include "memory.h"

#define printf(...) ((void)0)
#define fprintf(...) ((void)0)

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wunused-variable"
#  pragma clang diagnostic ignored "-Wunused-but-set-variable"
#  pragma clang diagnostic ignored "-Wunused-but-set-parameter"
#  pragma clang diagnostic ignored "-Wmissing-field-initializers"
#  pragma clang diagnostic ignored "-Wunused-function"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wunused-variable"
#  pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#  pragma GCC diagnostic ignored "-Wunused-but-set-parameter"
#  pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#  pragma GCC diagnostic ignored "-Wunused-function"
#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable: 4244)  // possible loss of data
#endif

#ifdef small
#  undef small  // on windows this seems to be defined to something...
#endif

#include "CRoaringUnityBuild/roaring.h"
#include "CRoaringUnityBuild/roaring.c"

#if defined(__clang__)
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif

#undef printf
#undef fprintf

static bool _roaringBitMemoryInitialized = false;
static std::mutex _roaringBitMemoryInitializedMutex;

static void croaringMemoryInitialize() {
  if (!_roaringBitMemoryInitialized) {
    std::lock_guard<std::mutex> guard(_roaringBitMemoryInitializedMutex);
    roaring_memory_t roaringMemory;
    roaringMemory.malloc = gcaware_malloc;
    roaringMemory.realloc = gcaware_realloc;
    roaringMemory.calloc = gcaware_calloc;
    roaringMemory.free = gcaware_free;
    roaringMemory.aligned_malloc = gcaware_aligned_malloc;
    roaringMemory.aligned_free = gcaware_aligned_free;
    roaring_init_memory_hook(roaringMemory);
    _roaringBitMemoryInitialized = true;
  }
}

#endif  // ROARING_NODE_CROARING_
