#ifndef __ROARINGBITMAP32ITERATOR__H__
#define __ROARINGBITMAP32ITERATOR__H__

#include "../RoaringBitmap32/RoaringBitmap32.h"

class RoaringBitmap32IteratorState : public v8utils::ObjectWrap {
 public:
  roaring_uint32_iterator_t it;
  RoaringBitmap32 * roaring;

  static v8::Persistent<v8::FunctionTemplate> constructorTemplate;
  static v8::Persistent<v8::Function> constructor;

  static void New(const v8::FunctionCallbackInfo<v8::Value> & info);

  static void done_getter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> & info);
  static void value_getter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> & info);

  RoaringBitmap32IteratorState();

 private:
  friend class RoaringBitmap32Iterator;
  static void Init(v8::Isolate * isolate);
};

class RoaringBitmap32Iterator {
 public:
  static v8::Persistent<v8::FunctionTemplate> constructorTemplate;
  static v8::Persistent<v8::Function> constructor;

  static void Init(v8::Local<v8::Object> exports);
  static void New(const v8::FunctionCallbackInfo<v8::Value> & info);
  static void next(const v8::FunctionCallbackInfo<v8::Value> & info);

  static void iterator_getter(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value> & info);

 private:
  RoaringBitmap32Iterator() = delete;
  ~RoaringBitmap32Iterator() = delete;
};

#endif