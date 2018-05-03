{
  "targets": [
    {
      "target_name": "roaring",
      "cflags": ["-Wall"],
      "include_dirs": ["src", "<!(node -e \"require('nan')\")"],
      "sources": ["src/cpp/RoaringBitmap32.cpp", "src/cpp/module.cpp", "src/cpp/roaring.cpp"]
    }
  ]
}
