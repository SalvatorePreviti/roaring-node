#ifndef __ROARINGBITMAP32BUFFEREDITERATOR__H__
#define __ROARINGBITMAP32BUFFEREDITERATOR__H__

#include "../RoaringBitmap32/RoaringBitmap32.h"

class RoaringBitmap32BufferedIterator {
 public:
  v8::Persistent<v8::Object> persistent;
  roaring_uint32_iterator_t it;
  uint64_t bitmapVersion;
  RoaringBitmap32 * bitmapInstance;
  v8utils::TypedArrayContent<uint32_t> bufferContent;

  v8::Persistent<v8::Object> buffer;
  v8::Persistent<v8::Object> bitmap;

  static v8::Eternal<v8::FunctionTemplate> constructorTemplate;
  static v8::Eternal<v8::Function> constructor;

  static void Init(v8::Local<v8::Object> exports);
  static void New(const v8::FunctionCallbackInfo<v8::Value> & info);

  static void fill(const v8::FunctionCallbackInfo<v8::Value> & info);

  RoaringBitmap32BufferedIterator();
  ~RoaringBitmap32BufferedIterator();

 private:
  void destroy();
  static v8::Eternal<v8::String> nPropertyName;
  static void WeakCallback(v8::WeakCallbackInfo<RoaringBitmap32BufferedIterator> const & info);
};

#endif