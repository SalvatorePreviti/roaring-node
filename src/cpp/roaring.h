#ifndef __ROARING__H__
#define __ROARING__H__

#define USESSE4 1

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define printf(...) (0)
#define fprintf(...) (0)

#include "CRoaringUnityBuild/roaring.h"

#undef printf
#undef fprintf

#endif