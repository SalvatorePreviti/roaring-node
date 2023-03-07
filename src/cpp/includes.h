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

#if NODE_MAJOR_VERSION > 14
#  define NEW_LITERAL_V8_STRING(isolate, str, type) v8::String::NewFromUtf8Literal(isolate, str, type)
#else
#  define NEW_LITERAL_V8_STRING(isolate, str, type) v8::String::NewFromUtf8(isolate, str, type).ToLocalChecked()
#endif

#endif  // ROARING_NODE_INCLUDES_
