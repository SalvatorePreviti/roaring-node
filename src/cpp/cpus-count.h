#ifndef ROARING_NODE_CPUS_COUNT_
#define ROARING_NODE_CPUS_COUNT_

#include "includes.h"

uint32_t getCpusCount() {
  static uint32_t _cpusCountCache = 0;

  uint32_t result = _cpusCountCache;
  if (result != 0) {
    return result;
  }

  uv_cpu_info_t * tmp = nullptr;
  int count = 0;
  uv_cpu_info(&tmp, &count);
  if (tmp != nullptr) {
    uv_free_cpu_info(tmp, count);
  }
  result = count <= 0 ? 1 : (uint32_t)count;
  _cpusCountCache = result;
  return result;
}

#endif  // ROARING_NODE_CPUS_COUNT_
