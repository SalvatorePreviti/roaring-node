"use strict";

/*
Copyright 2018 Salvatore Previti

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

Source code at: https://github.com/SalvatorePreviti/roaring-node 

Documentation at: https://salvatorepreviti.github.io/roaring-node/modules.html

Roaring Bitmap 32 documentation at: https://salvatorepreviti.github.io/roaring-node/classes/RoaringBitmap32.html

*/

const util = require("util");
const roaring = require("./build/Release/roaring.node");

module.exports = roaring;

const { defineProperty } = Reflect;

defineProperty(roaring, "__esModule", { value: true, configurable: true });

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

let isArrayBuffer;
let isArrayBufView;
let isSharedArrayBuffer;

// eslint-disable-next-line node/no-unsupported-features/node-builtins
const utilTypes = util.types; // util.types is supported only in Node.js 10+
if (utilTypes) {
  isArrayBuffer = utilTypes.isArrayBuffer;
  isArrayBufView = utilTypes.isArrayBufferView;
  isSharedArrayBuffer = utilTypes.isSharedArrayBuffer;
} else {
  isArrayBuffer = (v) => v instanceof ArrayBuffer;
  isArrayBufView = (v) => ArrayBuffer.isView(v);
  isSharedArrayBuffer = typeof SharedArrayBuffer === "undefined" ? () => false : (v) => v instanceof SharedArrayBuffer;
}

function asBufferUnsafe(buffer) {
  if (Buffer.isBuffer(buffer)) {
    return buffer;
  }
  if (isArrayBufView(buffer)) {
    return Buffer.from(buffer.buffer, buffer.byteOffset, buffer.byteLength);
  }
  if (isArrayBuffer(buffer)) {
    return Buffer.from(buffer);
  }
  if (isSharedArrayBuffer(buffer)) {
    return Buffer.from(buffer);
  }
  return buffer;
}

function asBuffer(buffer) {
  const result = asBufferUnsafe(buffer);
  return Buffer.isBuffer(result) ? result : Buffer.from(result);
}

const initializedSym = Symbol.for("roaring-node");

if (!roaring[initializedSym]) {
  Reflect.defineProperty(roaring, initializedSym, { value: true });

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
    let index = output.length;
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

  const defineProp = (name, prop) => {
    defineProperty(roaring, name, prop);
    defineProperty(RoaringBitmap32, name, prop);
    defineProperty(roaringBitmap32_proto, name, prop);
  };

  const defineValue = (name, value, writable) =>
    defineProp(name, { value, writable: !!writable, configurable: false, enumerable: false });

  let packageVersion = null;

  defineProp("PackageVersion", {
    get: () => (packageVersion !== null ? packageVersion : (packageVersion = require("./package.json").version)),
    configurable: false,
    enumerable: true,
  });

  defineValue("RoaringBitmap32Iterator", RoaringBitmap32Iterator, false);

  defineValue(
    "SerializationFormat",
    {
      croaring: "croaring",
      portable: "portable",
      unsafe_frozen_croaring: "unsafe_frozen_croaring",
    },
    false,
  );

  defineValue(
    "DeserializationFormat",
    {
      croaring: "croaring",
      portable: "portable",
      unsafe_frozen_croaring: "unsafe_frozen_croaring",
      unsafe_frozen_portable: "unsafe_frozen_portable",
    },
    false,
  );

  defineValue(
    "FrozenViewFormat",
    {
      unsafe_frozen_croaring: "unsafe_frozen_croaring",
      unsafe_frozen_portable: "unsafe_frozen_portable",
    },
    false,
  );

  defineValue("bufferAlignedAlloc", roaring.bufferAlignedAlloc);
  defineValue("bufferAlignedAllocUnsafe", roaring.bufferAlignedAllocUnsafe);
  defineValue("bufferAlignedAllocShared", roaring.bufferAlignedAllocShared);
  defineValue("bufferAlignedAllocSharedUnsafe", roaring.bufferAlignedAllocSharedUnsafe);
  defineValue("isBufferAligned", roaring.isBufferAligned);

  defineValue("ensureBufferAligned", function ensureBufferAligned(buffer, alignment = 32) {
    if (typeof alignment !== "number" || alignment < 0 || alignment > 1024 || !Number.isInteger(alignment)) {
      throw new TypeError("ensureBufferAligned alignment must be an integer number between 0 and 1024");
    }
    buffer = asBufferUnsafe(buffer);
    if (!Buffer.isBuffer(buffer)) {
      throw new TypeError("ensureBufferAligned expects a Buffer instance");
    }
    if (!roaring.isBufferAligned(buffer, alignment)) {
      const aligned = isSharedArrayBuffer(buffer.buffer)
        ? roaring.bufferAlignedAllocSharedUnsafe(buffer.length, alignment)
        : roaring.bufferAlignedAllocUnsafe(buffer.length, alignment);
      aligned.set(buffer);
      return aligned;
    }
    return buffer;
  });

  RoaringBitmap32.getRoaringUsedMemory = roaring.getRoaringUsedMemory;

  roaring.asBuffer = asBuffer;
}
