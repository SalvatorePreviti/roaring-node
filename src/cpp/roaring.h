#ifndef __ROARING__H__
#define __ROARING__H__

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define printf(...) ((void)0)
#define fprintf(...) ((void)0)

#include "CRoaringUnityBuild/roaring.h"

#undef printf
#undef fprintf

#endif