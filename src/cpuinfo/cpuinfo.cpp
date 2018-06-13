#include "node.h"

#if (defined(_MSC_VER)) || (defined(__INTEL_COMPILER))
#include <intrin.h>
#else

void _cpuid(unsigned int cpu_info[4U], const unsigned int cpu_info_type) {
  cpu_info[0] = cpu_info[1] = cpu_info[2] = cpu_info[3] = 0;

#if defined(_MSC_VER) && (defined(_M_X64) || defined(_M_AMD64) || defined(_M_IX86))
  __cpuid((int *)cpu_info, cpu_info_type);
#elif defined(__i386__)
  __asm__ __volatile__(
      "pushfl; pushfl; "
      "popl %0; "
      "movl %0, %1; xorl %2, %0; "
      "pushl %0; "
      "popfl; pushfl; popl %0; popfl"
      : "=&r"(cpu_info[0]), "=&r"(cpu_info[1])
      : "i"(0x200000));
  if (((cpu_info[0] ^ cpu_info[1]) & 0x200000) == 0x0) {
    return; /* LCOV_EXCL_LINE */
  }
#endif
#ifdef __i386__
  __asm__ __volatile__("xchgl %%ebx, %k1; cpuid; xchgl %%ebx, %k1"
                       : "=a"(cpu_info[0]), "=&r"(cpu_info[1]), "=c"(cpu_info[2]), "=d"(cpu_info[3])
                       : "0"(cpu_info_type), "2"(0U));
#elif defined(__x86_64__)
  __asm__ __volatile__("xchgq %%rbx, %q1; cpuid; xchgq %%rbx, %q1"
                       : "=a"(cpu_info[0]), "=&r"(cpu_info[1]), "=c"(cpu_info[2]), "=d"(cpu_info[3])
                       : "0"(cpu_info_type), "2"(0U));
#else
  __asm__ __volatile__("cpuid" : "=a"(cpu_info[0]), "=b"(cpu_info[1]), "=c"(cpu_info[2]), "=d"(cpu_info[3]) : "0"(cpu_info_type), "2"(0U));
#endif
}

void getSupportedInstructions(bool & sse42, bool & avx2) {
  unsigned int cpu_info[4];

  _cpuid(cpu_info, 0x0);

  unsigned int nids = cpu_info[0];

  if (nids > 0) {
    _cpuid(cpu_info, 0x00000001);
    if ((cpu_info[2] & (1 << 20)) && (cpu_info[2] & (1 << 23))) {
      sse42 = true;
    }
  }

  if (sse42 && nids >= 7) {
    _cpuid(cpu_info, 0x00000007);
    avx2 = (cpu_info[1] & (1 << 5)) != 0;
  }
}
#endif

void InitModule(v8::Local<v8::Object> exports) {
  bool sse42 = false;
  bool avx2 = false;

  getSupportedInstructions(sse42, avx2);

  v8::Isolate * isolate = v8::Isolate::GetCurrent();
  exports->Set(v8::String::NewFromUtf8(isolate, "SSE42"), v8::Boolean::New(isolate, sse42));
  exports->Set(v8::String::NewFromUtf8(isolate, "AVX2"), v8::Boolean::New(isolate, avx2));
}

NODE_MODULE(roaring, InitModule);
