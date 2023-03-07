#ifndef ROARING_NODE_MEMORY_
#define ROARING_NODE_MEMORY_

#include "includes.h"

/** portable version of posix_memalign */
static void * bare_aligned_malloc(size_t alignment, size_t size) {
  void * p;
#ifdef _MSC_VER
  p = _aligned_malloc(size, alignment);
#elif defined(__MINGW32__) || defined(__MINGW64__)
  p = __mingw_aligned_malloc(size, alignment);
#else
  // somehow, if this is used before including "x86intrin.h", it creates an
  // implicit defined warning.
  if (posix_memalign(&p, alignment, size) != 0) return NULL;
#endif
  return p;
}

/** portable version of free fo aligned allocs */
static void bare_aligned_free(void * memblock) {
  if (memblock != nullptr) {
#ifdef _MSC_VER
    _aligned_free(memblock);
#elif defined(__MINGW32__) || defined(__MINGW64__)
    __mingw_aligned_free(memblock);
#else
    free(memblock);
#endif
  }
}

/** portable version of malloc_size */
inline static size_t bare_malloc_size(const void * ptr) {
#if defined(__APPLE__)
  return malloc_size(ptr);
#elif defined(_WIN32) || defined(__MINGW32__) || defined(__MINGW64__)
  return _msize(ptr);
#else
  return malloc_usable_size(ptr);
#endif
}

/** portable version of malloc_size for memory allocated with bare_aligned_malloc */
inline static size_t bare_aligned_malloc_size(const void * ptr) {
#if defined(__APPLE__)
  return malloc_size(ptr);
#elif defined(_WIN32)
  return _aligned_msize(ptr, 32, 0);
#else
  return malloc_usable_size(ptr);
#endif
}

std::atomic<int64_t> gcaware_totalMemCounter{0};

int64_t gcaware_totalMem() { return gcaware_totalMemCounter; }

static thread_local v8::Isolate * thread_local_isolate = nullptr;

inline static void _gcaware_adjustAllocatedMemory(int64_t size) {
  if (size != 0) {
    v8::Isolate * isolate = v8::Isolate::GetCurrent();
    if (isolate == nullptr) {
      isolate = thread_local_isolate;
    }
    if (isolate != nullptr) {
      isolate->AdjustAmountOfExternalAllocatedMemory(size);
    }
    gcaware_totalMemCounter += size;
  }
}

inline static void gcaware_addAllocatedMemory(size_t size) { _gcaware_adjustAllocatedMemory((int64_t)size); }

inline static void gcaware_removeAllocatedMemory(size_t size) { _gcaware_adjustAllocatedMemory(-(int64_t)size); }

static void * gcaware_malloc(size_t size) {
  void * memory = malloc(size);
  if (memory != nullptr) {
    gcaware_addAllocatedMemory(bare_malloc_size(memory));
  }
  return memory;
}

static void * gcaware_realloc(void * memory, size_t size) {
  size_t oldSize = memory != nullptr ? bare_malloc_size(memory) : 0;
  memory = realloc(memory, size);
  if (memory != nullptr) {
    gcaware_removeAllocatedMemory(oldSize);
    gcaware_addAllocatedMemory(bare_malloc_size(memory));
  }
  return memory;
}

static void * gcaware_calloc(size_t count, size_t size) {
  void * memory = calloc(count, size);
  if (memory != nullptr) {
    gcaware_addAllocatedMemory(bare_malloc_size(memory));
  }
  return memory;
}

static void gcaware_free(void * memory) {
  if (memory != nullptr) {
    gcaware_removeAllocatedMemory(bare_malloc_size(memory));
  }
  free(memory);
}

static void * gcaware_aligned_malloc(size_t alignment, size_t size) {
  void * memory = bare_aligned_malloc(alignment, size);
  if (memory != nullptr) {
    gcaware_addAllocatedMemory(bare_aligned_malloc_size(memory));
  }
  return memory;
}

static void gcaware_aligned_free(void * memory) {
  if (memory != nullptr) {
    gcaware_removeAllocatedMemory(bare_aligned_malloc_size(memory));
  }
  bare_aligned_free(memory);
}

static void bare_aligned_free_callback(char * data, void * hint) { bare_aligned_free(data); }

#endif  // ROARING_NODE_MEMORY_
