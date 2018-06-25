#ifndef __ROARING_ATOMIC__H__
#define __ROARING_ATOMIC__H__

#ifdef _MSC_VER
#  define atomicIncrement32(volatile_uint32_ptr) InterlockedIncrement(volatile_uint32_ptr)
#else
#  define atomicIncrement32(volatile_uint32_ptr) __sync_add_and_fetch(volatile_uint32_ptr, 1)
#endif

#endif