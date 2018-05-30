#ifndef __ROARINGBITMAP32ITERATOR__H__
#define __ROARINGBITMAP32ITERATOR__H__

#include "./RoaringBitmap32.h"

class RoaringBitmap32Iterator : public Nan::ObjectWrap {
 public:
  roaring_uint32_iterator_t it;
  RoaringBitmap32 * roaring;
  v8::Persistent<v8::Value> bitmap;

  static Nan::Persistent<v8::FunctionTemplate> constructorTemplate;
  static Nan::Persistent<v8::Function> constructor;

  static void Init(v8::Local<v8::Object> exports);
  static void New(const Nan::FunctionCallbackInfo<v8::Value> & info);
  static void next(const Nan::FunctionCallbackInfo<v8::Value> & info);

  static NAN_PROPERTY_GETTER(namedPropertyGetter);

  RoaringBitmap32Iterator();
  virtual ~RoaringBitmap32Iterator();
};

#endif