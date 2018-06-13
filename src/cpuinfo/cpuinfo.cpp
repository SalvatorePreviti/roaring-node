#include "node.h"

// unless DISABLEAVX was defined, if we have __AVX2__, we enable AVX
#if (!defined(USEAVX)) && (!defined(DISABLEAVX)) && (defined(__AVX2__))
#define USEAVX
#define USESSE4
#endif

#if (defined(__POPCNT__)) && (defined(__SSE4_2__))
#define USESSE4
#endif

#if (defined(USESSE4) || defined(USEAVX))
#define MUST_CHECK_CPUID
#ifdef _MSC_VER
#include <intrin.h>
void getSupportedInstructions(bool & sse42, bool & avx2) {
  int d[4] = {0, 0, 0, 0};
  __cpuid(d, 1);
  if (((d[0] >> 8) & 15) >= 6) {
    avx2 = (((d[2] & (1 << 28)) != 0) && ((d[2] & (1 << 27)) != 0));
    sse42 = avx2 || ((d[2] & (1 << 20)) != 0);
  }
}
#else
#include <cpuid.h>
void getSupportedInstructions(bool & sse42, bool & avx2) {
  int d[4] = {0, 0, 0, 0};
  __cpuid(1, d[0], d[1], d[2], d[3]);
  if (((d[0] >> 8) & 15) >= 6) {
    avx2 = (((d[2] & (1 << 28)) != 0) && ((d[2] & (1 << 27)) != 0));
    sse42 = avx2 || ((d[2] & (1 << 20)) != 0);
  }
}
#endif
#else
#define getSupportedInstructions(...) ((void)0)
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
