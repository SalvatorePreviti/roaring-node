#include "roaring.h"

#define printf(...) ((void)0)
#define fprintf(...) ((void)0)

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wunused-variable"
#endif

#include "CRoaringUnityBuild/roaring.c"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#if defined(__clang__)
#  pragma clang diagnostic pop
#endif

#undef printf
#undef fprintf
