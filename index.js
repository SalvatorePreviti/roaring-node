"use strict";

const roaring = require("./build/Release/roaring.node");
const { version: packageVersion } = require("./package.json");

module.exports = roaring;

const { defineProperty } = Reflect;

defineProperty(roaring, "__esModule", { value: true });

const { RoaringBitmap32, RoaringBitmap32BufferedIterator } = roaring;

class RoaringBitmap32IteratorResult {
  constructor() {
    this.value = undefined;
    this.done = true;
  }
}

const _iteratorBufferPoolMax = 32;
const _iteratorBufferPoolDefaultLen = 512;
const _iteratorBufferPool = [];

class RoaringBitmap32Iterator {
  constructor(bitmap, buffer = _iteratorBufferPoolDefaultLen) {
    const r = new RoaringBitmap32IteratorResult();
    let t;
    let i = 0;
    let n = 0;

    if (!(bitmap instanceof RoaringBitmap32)) {
      if (bitmap === undefined || bitmap === null) {
        t = null;
      } else {
        throw new TypeError("RoaringBitmap32Iterator constructor expects a RoaringBitmap32 instance");
      }
    }

    const init = () => {
      if (typeof buffer === "number") {
        buffer = (buffer === _iteratorBufferPoolDefaultLen && _iteratorBufferPool.pop()) || new Uint32Array(buffer);
      }
      t = new RoaringBitmap32BufferedIterator(bitmap, buffer);
      n = t.n;
      r.done = false;
    };

    const m = () => {
      if (t !== null) {
        if (t === undefined) {
          init();
        } else {
          n = t.fill();
        }

        if (n === 0) {
          t = null;
          if (
            buffer &&
            _iteratorBufferPool.length < _iteratorBufferPoolMax &&
            buffer.length === _iteratorBufferPoolDefaultLen
          ) {
            _iteratorBufferPool.push(buffer);
          }
          buffer = undefined;
          bitmap = undefined;
          r.value = undefined;
          r.done = true;
        } else {
          i = 1;
          r.value = buffer[0];
        }
      }
    };

    this.next = () => {
      if (i < n) {
        r.value = buffer[i++];
      } else {
        m();
      }
      return r;
    };
  }

  [Symbol.iterator]() {
    return this;
  }
}

const roaringBitmap32IteratorProp = {
  value: RoaringBitmap32Iterator,
  writable: false,
  configurable: false,
  enumerable: false,
};

defineProperty(RoaringBitmap32Iterator, "default", roaringBitmap32IteratorProp);
defineProperty(RoaringBitmap32Iterator, "RoaringBitmap32Iterator", roaringBitmap32IteratorProp);

function iterator() {
  return new RoaringBitmap32Iterator(this);
}

if (!roaring.PackageVersion) {
  const roaringBitmap32_proto = RoaringBitmap32.prototype;
  roaringBitmap32_proto[Symbol.iterator] = iterator;
  roaringBitmap32_proto.iterator = iterator;
  roaringBitmap32_proto.keys = iterator;
  roaringBitmap32_proto.values = iterator;
  roaringBitmap32_proto.entries = function* entries() {
    for (const v of this) {
      yield [v, v];
    }
  };

  roaringBitmap32_proto.forEach = function (fn, self) {
    if (self === undefined) {
      for (const v of this) {
        fn(v, v, this);
      }
    } else {
      for (const v of this) {
        fn.call(self, v, v, this);
      }
    }
    return this;
  };

  roaringBitmap32_proto.map = function (fn, self, output = []) {
    let index = 0;
    if (self === undefined) {
      for (const v of this) {
        output.push(fn(v, index++, this));
      }
    } else {
      for (const v of this) {
        output.push(fn.call(self, v, index++, this));
      }
    }
    return output;
  };

  roaringBitmap32_proto.toJSON = function () {
    return this.toArray();
  };

  const define = (name, value, writable) => {
    const prop = {
      value,
      writable: !!writable,
      configurable: false,
      enumerable: false,
    };
    defineProperty(roaring, name, prop);
    defineProperty(RoaringBitmap32, name, prop);
    defineProperty(roaringBitmap32_proto, name, prop);
  };

  define("PackageVersion", packageVersion);
  define("RoaringBitmap32Iterator", RoaringBitmap32Iterator, false);
  define(
    "SerializationFormat",
    {
      croaring: "croaring",
      portable: "portable",
      frozen_croaring: "frozen_croaring",
    },
    false,
  );

  define("bufferAlignedAlloc", roaring.bufferAlignedAlloc);
  define("bufferAlignedAllocUnsafe", roaring.bufferAlignedAllocUnsafe);
  define("bufferIsAligned", roaring.bufferIsAligned);

  roaring._initTypes({ Uint32Array });
}
