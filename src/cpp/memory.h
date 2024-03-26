#ifndef ROARING_NODE_MEMORY_
#define ROARING_NODE_MEMORY_

#include "core.h"
#include "includes.h"

std::atomic<int64_t> gcaware_totalMemCounter{0};

int64_t gcaware_totalMem() { return gcaware_totalMemCounter; }

thread_local v8::Isolate * thread_local_isolate = nullptr;

inline void _gcaware_adjustAllocatedMemory(int64_t size) {
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

inline void gcaware_addAllocatedMemory(size_t size) { _gcaware_adjustAllocatedMemory((int64_t)size); }

inline void gcaware_removeAllocatedMemory(size_t size) { _gcaware_adjustAllocatedMemory(-(int64_t)size); }

void * gcaware_malloc(size_t size) {
  void * memory = malloc(size);
  if (memory != nullptr) {
    gcaware_addAllocatedMemory(bare_malloc_size(memory));
  }
  return memory;
}

void * gcaware_realloc(void * memory, size_t size) {
  size_t oldSize = memory != nullptr ? bare_malloc_size(memory) : 0;
  memory = realloc(memory, size);
  if (memory != nullptr) {
    gcaware_removeAllocatedMemory(oldSize);
    gcaware_addAllocatedMemory(bare_malloc_size(memory));
  }
  return memory;
}

void * gcaware_calloc(size_t count, size_t size) {
  void * memory = calloc(count, size);
  if (memory != nullptr) {
    gcaware_addAllocatedMemory(bare_malloc_size(memory));
  }
  return memory;
}

void gcaware_free(void * memory) {
  if (memory != nullptr) {
    gcaware_removeAllocatedMemory(bare_malloc_size(memory));
    free(memory);
  }
}

void * gcaware_aligned_malloc(size_t alignment, size_t size) {
  if (size == 0) {
    size = 1;
  }
  void * memory = bare_aligned_malloc(alignment, size);
  if (memory != nullptr) {
    gcaware_addAllocatedMemory(bare_aligned_malloc_size(memory));
  }
  return memory;
}

void gcaware_aligned_free(void * memory) {
  if (memory != nullptr) {
    gcaware_removeAllocatedMemory(bare_aligned_malloc_size(memory));
    bare_aligned_free(memory);
  }
}

void bare_aligned_free_callback(char * data, void * hint) { bare_aligned_free(hint); }

void bare_aligned_free_callback2(void * data, size_t length, void * deleter_data) { bare_aligned_free(deleter_data); }

inline bool is_pointer_aligned(const void * ptr, std::uintptr_t alignment) noexcept {
  auto iptr = reinterpret_cast<std::uintptr_t>(ptr);
  return !(iptr % alignment);
}

#endif  // ROARING_NODE_MEMORY_
