#ifndef ROARING_NODE_CORE_
#define ROARING_NODE_CORE_

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <atomic>
#include <string.h>
#include <math.h>
#include <cmath>
#include <limits>
#include <string>

#if defined(__APPLE__)
#  include <malloc/malloc.h>
#else
#  include <malloc.h>
#endif

#ifdef _MSC_VER
#  define atomicIncrement32(ptr) InterlockedIncrement(ptr)
#  define atomicDecrement32(ptr) InterlockedDecrement(ptr)
#else
#  define atomicIncrement32(ptr) __sync_add_and_fetch(ptr, 1)
#  define atomicDecrement32(ptr) __sync_sub_and_fetch(ptr, 1)
#endif

typedef const char * const_char_ptr_t;

/** portable version of posix_memalign */
void * bare_aligned_malloc(size_t alignment, size_t size) {
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
void bare_aligned_free(void * memblock) {
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
inline size_t bare_malloc_size(const void * ptr) {
#if defined(__APPLE__)
  return malloc_size(const_cast<void *>(ptr));
#elif defined(_WIN32) || defined(__MINGW32__) || defined(__MINGW64__)
  return _msize(const_cast<void *>(ptr));
#else
  return malloc_usable_size(const_cast<void *>(ptr));
#endif
}

/** portable version of malloc_size for memory allocated with bare_aligned_malloc */
inline size_t bare_aligned_malloc_size(const void * ptr) {
#if defined(__APPLE__)
  return malloc_size(const_cast<void *>(ptr));
#elif defined(_WIN32)
  return _aligned_msize(const_cast<void *>(ptr), 32, 0);
#else
  return malloc_usable_size(const_cast<void *>(ptr));
#endif
}

#endif
