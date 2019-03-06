#include "node.h"

#if (!defined(DISABLE_X64)) && (defined(__x86_64__) || defined(IS_X64) || defined(_M_X64) || defined(_M_AMD64))
#  define USE_CPUID 1
#endif

#if defined(USE_CPUID) && defined(_MSC_VER)
#  define USE_INTRIN_H 1
#  include <immintrin.h>  // For _xgetbv()
#  include <intrin.h>  // For __cpuid
#endif

#if defined(USE_INTRIN_H)
static void _cpuid(uint32_t info[4U], const uint32_t info_type) {
  info[0] = info[1] = info[2] = info[3] = 0;
  __cpuid((int *)info, info_type);
}
static uint64_t xgetbv(uint32_t xcr) {
  return _xgetbv(xcr);
}
#elif defined(USE_CPUID)
static void _cpuid(uint32_t info[4U], const uint32_t info_type) {
  info[0] = info[1] = info[2] = info[3] = 0u;
  __asm__ __volatile__("xchgq %%rbx, %q1; cpuid; xchgq %%rbx, %q1" : "=a"(info[0]), "=&r"(info[1]), "=c"(info[2]), "=d"(info[3]) : "0"(info_type), "2"(0U));
}
static uint64_t xgetbv(uint32_t xcr) {
  uint32_t eax = 0u;
  uint32_t edx = 0u;
  __asm__(".byte 0x0f, 0x01, 0xd0" : "=a"(eax), "=d"(edx) : "c"(xcr));
  return (uint64_t)(edx) << 32 | eax;
}
#else
static void _cpuid(uint32_t info[4U], const uint32_t info_type) {
  info[0] = info[1] = info[2] = info[3] = 0;
}
static uint64_t xgetbv(uint32_t) {
  return 0;
}
#endif

const uint32_t AVX = 1u << 28;
const uint32_t OSXSAVE = 1u << 27;
const uint32_t XSAVE = 1u << 26;
const uint32_t XMM_STATE = 1u << 1;
const uint32_t YMM_STATE = 1u << 2;
const uint32_t SSE42_BITS = (1u << 20) | (1u << 23);
const uint32_t AVX_BITS = AVX | OSXSAVE | XSAVE;
const uint32_t AVX2_BITS = (1u << 5);
const uint32_t XMM_YMM_STATE_BITS = XMM_STATE | YMM_STATE;

enum CPUIDRegister { eax = 0, ebx = 1, ecx = 2, edx = 3 };

enum CPUSupport { cpu_none = 0, cpu_sse42 = 1, cpu_avx = 2, cpu_avx2 = 4 };

inline static CPUSupport getSupportedInstructions() {
  uint32_t regs[4];
  _cpuid(regs, 0u);

  uint32_t level = regs[eax];
  if (!level) {
    return cpu_none;
  }

  CPUSupport sse42 = cpu_none;
  CPUSupport avx = cpu_none;
  CPUSupport avx2 = cpu_none;

  _cpuid(regs, 1u);
  const uint32_t extensionBits = regs[ecx];

  if ((extensionBits & SSE42_BITS) == SSE42_BITS) {
    sse42 = cpu_sse42;
  }

  if (sse42 && (extensionBits & AVX_BITS) == AVX_BITS) {
    // ensure the OS supports AVX (XSAVE of YMM registers)
    if ((xgetbv(0) & XMM_YMM_STATE_BITS) == XMM_YMM_STATE_BITS) {
      avx = cpu_avx;
    }
  }

  if (avx && level >= 7u) {
    _cpuid(regs, 7u);
    if ((regs[ebx] & AVX2_BITS) == AVX2_BITS) {
      avx2 = cpu_avx2;
    }
  }

  return static_cast<CPUSupport>(sse42 | avx | avx2);
}

void InitModule(v8::Local<v8::Object> exports) {
  CPUSupport support = getSupportedInstructions();

  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  exports->Set(v8::String::NewFromUtf8(isolate, "SSE42"), v8::Boolean::New(isolate, (support & cpu_sse42) != 0));
  exports->Set(v8::String::NewFromUtf8(isolate, "AVX2"), v8::Boolean::New(isolate, (support & cpu_avx2) != 0));
}

NODE_MODULE(roaring, InitModule);
