{
  "target_defaults": {
    "include_dirs": ["src", "<!(node -e \"require('nan')\")"],
    "cflags": ["-Wall", "-O3"],
    "cflags_cc": ["-Wall", "-O3"]
  },
  "targets": [
    {
      "target_name": "roaring",
      "sources": [
        "src/cpp/module.cpp",
        "src/cpp/TypedArrays.cpp",
        "src/cpp/RoaringBitmap32/RoaringBitmap32.cpp",
        "src/cpp/RoaringBitmap32/RoaringBitmap32_comparisons.cpp",
        "src/cpp/RoaringBitmap32/RoaringBitmap32_operations.cpp",
        "src/cpp/RoaringBitmap32/RoaringBitmap32_serialization.cpp",
        "src/cpp/RoaringBitmap32Iterator/RoaringBitmap32Iterator.cpp",
        "src/cpp/roaring.cpp"
      ]
    }
  ]
}
