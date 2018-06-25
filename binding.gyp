{
  "targets": [
    {
      "target_name": "roaring",
      "default_configuration": "Release",
      "cflags": ["-O3", "-std=c99"],
      "cflags_cc": ["-O3", "-std=c++11"],
      "defines": ["DISABLEAVX"],
      "sources": [
        "src/cpp/roaring.c",
        "src/cpp/module.cpp",
        "src/cpp/v8utils/v8utils.cpp",
        "src/cpp/RoaringBitmap32/RoaringBitmap32.cpp",
        "src/cpp/RoaringBitmap32/RoaringBitmap32_comparisons.cpp",
        "src/cpp/RoaringBitmap32/RoaringBitmap32_operations.cpp",
        "src/cpp/RoaringBitmap32/RoaringBitmap32_serialization.cpp",
        "src/cpp/RoaringBitmap32BufferedIterator/RoaringBitmap32BufferedIterator.cpp"
      ]
    }
  ],
  "conditions": [
    [
      "target_arch in \"x64 x86_64\"",
      {
        "targets": [
          {
            "target_name": "cpuinfo",
            "default_configuration": "Release",
            "cflags": ["-O3"],
            "cflags_cc": ["-O3", "-std=c++11"],
            "sources": ["src/cpuinfo/cpuinfo.cpp"]
          },
          {
            "target_name": "roaring-sse42",
            "default_configuration": "Release",
            "cflags": ["-O3", "-std=c99", "-msse4.2"],
            "cflags_cc": ["-O3", "-std=c++11", "-msse4.2"],
            "defines": ["DISABLEAVX", "__POPCNT__", "__SSE4_2__"],
            "xcode_settings": {
              "GCC_ENABLE_SSE42_EXTENSIONS": "YES",
              "OTHER_CFLAGS": ["-msse4.2"]
            },
            "sources": [
              "src/cpp/roaring.c",
              "src/cpp/module.cpp",
              "src/cpp/v8utils/v8utils.cpp",
              "src/cpp/RoaringBitmap32/RoaringBitmap32.cpp",
              "src/cpp/RoaringBitmap32/RoaringBitmap32_comparisons.cpp",
              "src/cpp/RoaringBitmap32/RoaringBitmap32_operations.cpp",
              "src/cpp/RoaringBitmap32/RoaringBitmap32_serialization.cpp",
              "src/cpp/RoaringBitmap32BufferedIterator/RoaringBitmap32BufferedIterator.cpp"
            ]
          },
          {
            "target_name": "roaring-avx2",
            "default_configuration": "Release",
            "cflags": ["-O3", "-std=c99", "-mavx2"],
            "cflags_cc": ["-O3", "-std=c++11", "-mavx2"],
            "defines": ["USEAVX", "__POPCNT__", "__SSE4_2__"],
            "xcode_settings": {
              "GCC_ENABLE_SSE42_EXTENSIONS": "YES",
              "CLANG_X86_VECTOR_INSTRUCTIONS": "avx2",
              "OTHER_CFLAGS": ["-mavx2"]
            },
            "msvs_settings": {
              "VCCLCompilerTool": {
                "AdditionalOptions": ["/arch:AVX2"]
              }
            },
            "sources": [
              "src/cpp/roaring.c",
              "src/cpp/module.cpp",
              "src/cpp/v8utils/v8utils.cpp",
              "src/cpp/RoaringBitmap32/RoaringBitmap32.cpp",
              "src/cpp/RoaringBitmap32/RoaringBitmap32_comparisons.cpp",
              "src/cpp/RoaringBitmap32/RoaringBitmap32_operations.cpp",
              "src/cpp/RoaringBitmap32/RoaringBitmap32_serialization.cpp",
              "src/cpp/RoaringBitmap32BufferedIterator/RoaringBitmap32BufferedIterator.cpp"
            ]
          }
        ]
      }
    ]
  ]
}
