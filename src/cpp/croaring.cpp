#define printf(...) ((void)0)
#define fprintf(...) ((void)0)

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wunused-variable"
#  pragma clang diagnostic ignored "-Wunused-but-set-variable"
#  pragma clang diagnostic ignored "-Wunused-but-set-parameter"
#  pragma clang diagnostic ignored "-Wmissing-field-initializers"
#  pragma clang diagnostic ignored "-Wunused-function"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wunused-variable"
#  pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#  pragma GCC diagnostic ignored "-Wunused-but-set-parameter"
#  pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#  pragma GCC diagnostic ignored "-Wunused-function"
#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable: 4244)  // possible loss of data
#endif

#ifdef small
#  undef small  // on windows this seems to be defined to something...
#endif

#include "../../submodules/CRoaring/src/isadetection.c"
#include "../../submodules/CRoaring/src/array_util.c"
#include "../../submodules/CRoaring/src/bitset_util.c"
#include "../../submodules/CRoaring/src/bitset.c"
#include "../../submodules/CRoaring/src/containers/array.c"
#include "../../submodules/CRoaring/src/containers/bitset.c"
#include "../../submodules/CRoaring/src/containers/containers.c"
#include "../../submodules/CRoaring/src/containers/convert.c"
#include "../../submodules/CRoaring/src/containers/mixed_intersection.c"
#include "../../submodules/CRoaring/src/containers/mixed_union.c"
#include "../../submodules/CRoaring/src/containers/mixed_equal.c"
#include "../../submodules/CRoaring/src/containers/mixed_subset.c"
#include "../../submodules/CRoaring/src/containers/mixed_negation.c"
#include "../../submodules/CRoaring/src/containers/mixed_xor.c"
#include "../../submodules/CRoaring/src/containers/mixed_andnot.c"
#include "../../submodules/CRoaring/src/containers/run.c"
#include "../../submodules/CRoaring/src/memory.c"
#include "../../submodules/CRoaring/src/roaring.c"
#include "../../submodules/CRoaring/src/roaring_priority_queue.c"
#include "../../submodules/CRoaring/src/roaring_array.c"

#if defined(__clang__)
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif

#undef printf
#undef fprintf
