#ifndef __ROARINGBITMAP32BUFFEREDITERATOR__H__
#define __ROARINGBITMAP32BUFFEREDITERATOR__H__

#include "../RoaringBitmap32/RoaringBitmap32.h"

class RoaringBitmap32BufferedIterator : public v8utils::ObjectWrap {
 public:
  roaring_uint32_iterator_t it;
  v8::Persistent<v8::Object> buffer;
  v8::Persistent<v8::Object> bitmap;

  static v8::Persistent<v8::FunctionTemplate> constructorTemplate;
  static v8::Persistent<v8::Function> constructor;

  static void Init(v8::Local<v8::Object> exports);
  static void New(const v8::FunctionCallbackInfo<v8::Value> & info);

  static void fill(const v8::FunctionCallbackInfo<v8::Value> & info);

  RoaringBitmap32BufferedIterator();
  virtual ~RoaringBitmap32BufferedIterator();

 private:
};

#endif