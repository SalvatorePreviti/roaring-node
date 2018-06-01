#include "roaring.h"

#define printf(...) (0)
#define fprintf(...) (0)

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#endif

#include "CRoaringUnityBuild/roaring.c"

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#undef printf
#undef fprintf
