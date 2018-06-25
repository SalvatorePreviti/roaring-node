#ifndef __ROARING_ATOMIC__H__
#define __ROARING_ATOMIC__H__

#ifdef _MSC_VER
#  define atomicIncrement32(ptr) InterlockedIncrement(ptr)
#  define atomicDecrement32(ptr) InterlockedDecrement(ptr)
#else
#  define atomicIncrement32(ptr) __sync_add_and_fetch(ptr, 1)
#  define atomicDecrement32(ptr) __sync_sub_and_fetch(ptr, 1)
#endif

#endif