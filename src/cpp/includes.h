#ifndef ROARING_NODE_INCLUDES_
#define ROARING_NODE_INCLUDES_

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <node.h>
#include <node_buffer.h>
#include <uv.h>
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

#include "croaring.h"

typedef const char * const_char_ptr_t;

#endif  // ROARING_NODE_INCLUDES_
