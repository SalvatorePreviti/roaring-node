#ifndef __ROARING__H__
#define __ROARING__H__

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define printf(...) ((void)0)
#define fprintf(...) ((void)0)

#include "CRoaringUnityBuild/roaring.h"

#define ROARING_VERSION_STRING_TOK(x, y) #x
#define ROARING_VERSION_STRING_BUILD(x) ROARING_VERSION_STRING_TOK(x)
#define ROARING_VERSION_STRING ((ROARING_VERSION_STRING_BUILD(ROARING_VERSION)) + 2)

#undef printf
#undef fprintf

#endif
