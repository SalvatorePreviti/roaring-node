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

// TODO: remove this once latest version is merged in CRoaringUnityBuild
bool roaring_bitmap_add_checked(roaring_bitmap_t * r, uint32_t val) {
  const uint16_t hb = val >> 16;
  const int i = ra_get_index(&r->high_low_container, hb);
  uint8_t typecode;
  bool result = false;
  if (i >= 0) {
    ra_unshare_container_at_index(&r->high_low_container, i);
    void * container = ra_get_container_at_index(&r->high_low_container, i, &typecode);

    const int oldCardinality = container_get_cardinality(container, typecode);

    uint8_t newtypecode = typecode;
    void * container2 = container_add(container, val & 0xFFFF, typecode, &newtypecode);
    if (container2 != container) {
      container_free(container, typecode);
      ra_set_container_at_index(&r->high_low_container, i, container2, newtypecode);
      result = true;
    } else {
      const int newCardinality = container_get_cardinality(container, newtypecode);

      result = oldCardinality != newCardinality;
    }
  } else {
    array_container_t * newac = array_container_create();
    void * container = container_add(newac, val & 0xFFFF, ARRAY_CONTAINER_TYPE_CODE, &typecode);
    // we could just assume that it stays an array container
    ra_insert_new_key_value_at(&r->high_low_container, -i - 1, hb, container, typecode);
    result = true;
  }

  return result;
}

// TODO: remove this once latest version is merged in CRoaringUnityBuild
bool roaring_bitmap_remove_checked(roaring_bitmap_t * r, uint32_t val) {
  const uint16_t hb = val >> 16;
  const int i = ra_get_index(&r->high_low_container, hb);
  uint8_t typecode;
  bool result = false;
  if (i >= 0) {
    ra_unshare_container_at_index(&r->high_low_container, i);
    void * container = ra_get_container_at_index(&r->high_low_container, i, &typecode);

    const int oldCardinality = container_get_cardinality(container, typecode);

    uint8_t newtypecode = typecode;
    void * container2 = container_remove(container, val & 0xFFFF, typecode, &newtypecode);
    if (container2 != container) {
      container_free(container, typecode);
      ra_set_container_at_index(&r->high_low_container, i, container2, newtypecode);
    }

    const int newCardinality = container_get_cardinality(container2, newtypecode);

    if (newCardinality != 0) {
      ra_set_container_at_index(&r->high_low_container, i, container2, newtypecode);
    } else {
      ra_remove_at_index_and_free(&r->high_low_container, i);
    }

    result = oldCardinality != newCardinality;
  }
  return result;
}

#ifdef __cplusplus
}
#endif

#if defined(__clang__)
#  pragma clang diagnostic pop
#endif

#undef printf
#undef fprintf
