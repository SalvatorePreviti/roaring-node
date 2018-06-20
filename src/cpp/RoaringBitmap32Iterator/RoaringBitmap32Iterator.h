#ifndef __ROARINGBITMAP32ITERATOR__H__
#define __ROARINGBITMAP32ITERATOR__H__

#include "../RoaringBitmap32/RoaringBitmap32.h"

class RoaringBitmap32Iterator : public v8utils::ObjectWrap {
 public:
  roaring_uint32_iterator_t it;
  RoaringBitmap32 * roaring;
  v8::Persistent<v8::Value> bitmap;

  static v8::Persistent<v8::FunctionTemplate> constructorTemplate;
  static v8::Persistent<v8::Function> constructor;

  static void Init(v8::Local<v8::Object> exports);
  static void New(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void next(const v8::FunctionCallbackInfo<v8::Value> & info);

  static NAN_PROPERTY_GETTER(namedPropertyGetter);

  RoaringBitmap32Iterator();
  virtual ~RoaringBitmap32Iterator();
};

#endif