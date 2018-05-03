#ifndef __ROARINGBITMAP32__H__
#define __ROARINGBITMAP32__H__

#include <nan.h>
#include "roaring.h"

class RoaringBitmap32 : public Nan::ObjectWrap {
 public:
  roaring_bitmap_t roaring;

  static Nan::Persistent<v8::FunctionTemplate> constructor;

  static NAN_MODULE_INIT(Init);

  static NAN_METHOD(New);

  static NAN_PROPERTY_GETTER(isEmpty_getter);
  static NAN_PROPERTY_GETTER(size_getter);

  static NAN_METHOD(has);
  static NAN_METHOD(add);
  static NAN_METHOD(tryAdd);
  static NAN_METHOD(addMany);
  static NAN_METHOD(remove);
  static NAN_METHOD(removeMany);
  static NAN_METHOD(removeChecked);
  static NAN_METHOD(clear);

  static NAN_METHOD(minimum);
  static NAN_METHOD(maximum);

  RoaringBitmap32();

  virtual ~RoaringBitmap32();
};

#endif