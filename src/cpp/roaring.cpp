#include "roaring.h"

#define printf(...) (0)
#define fprintf(...) (0)

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"

#include "CRoaringUnityBuild/roaring.c"

#pragma clang diagnostic pop

#undef printf
#undef fprintf
