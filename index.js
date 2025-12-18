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

const path = require("node:path");
const util = require("node:util");

const roaring = (() => {
  try {
    return require(
      require("@mapbox/node-pre-gyp/lib/pre-binding").find(path.resolve(path.join(__dirname, "package.json"))),
    );
  } catch (e) {
    if (e && e.code === "MODULE_NOT_FOUND") {
      return require("./build/Release/roaring.node");
    }
    throw e;
  }
})();

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
const _iteratorBufferPoolDefaultLen = 2048;
const _iteratorBufferPool = new Array(_iteratorBufferPoolMax);
let _iteratorBufferPoolLen = 0;
const _symbolDispose = typeof Symbol.dispose === "symbol" ? Symbol.dispose : undefined;

function normalizeIteratorChunkLength(length) {
  const n = Number(length);
  if (!Number.isFinite(n) || n <= 0) {
    return _iteratorBufferPoolDefaultLen;
  }
  if (n > 16777216) {
    return 16777216;
  }
  const normalized = Math.floor(n);
  return normalized > 0 ? normalized : 1;
}

function acquireIteratorBuffer(length) {
  if (length === _iteratorBufferPoolDefaultLen && _iteratorBufferPoolLen) {
    return _iteratorBufferPool[--_iteratorBufferPoolLen];
  }
  return new Uint32Array(length);
}

function releaseIteratorBuffer(buffer) {
  if (_iteratorBufferPoolLen < _iteratorBufferPoolMax) {
    _iteratorBufferPool[_iteratorBufferPoolLen++] = buffer;
  }
}

function normalizeIteratorInputs(bitmap, buffer) {
  let done = false;
  if (bitmap instanceof RoaringBitmap32) {
    done = false;
  } else if (bitmap === undefined || bitmap === null) {
    bitmap = undefined;
    done = true;
  } else {
    throw new TypeError("RoaringBitmap32Iterator constructor expects a RoaringBitmap32 instance");
  }

  let chunk = null;
  let bufferLength = 0;
  let bufferReusable = false;
  if (!done) {
    if (typeof buffer === "number") {
      bufferLength = normalizeIteratorChunkLength(buffer);
      bufferReusable = bufferLength === _iteratorBufferPoolDefaultLen;
    } else if (buffer instanceof Uint32Array) {
      if (buffer.length === 0) {
        throw new TypeError("RoaringBitmap32Iterator buffer must have a positive length");
      }
      chunk = buffer;
      bufferLength = buffer.length;
      bufferReusable = false;
    } else if (buffer === undefined) {
      bufferLength = _iteratorBufferPoolDefaultLen;
      bufferReusable = true;
    } else {
      throw new TypeError("RoaringBitmap32Iterator buffer must be a number or a Uint32Array");
    }
  }

  return { bitmap, chunk, bufferLength, bufferReusable, done };
}

function defineRoaringBitmap32Iterator(reverse, name) {
  function Iterator(bitmap, buffer = _iteratorBufferPoolDefaultLen) {
    if (!new.target) {
      return new Iterator(bitmap, buffer);
    }

    var result = null;
    var reader = null;
    var chunk = null;
    var bufferReusable = false;
    var bufferLength = 0;
    var bufferIndex = 0;
    var bufferCount = 0;
    var done = false;

    ({ bitmap, chunk, bufferLength, bufferReusable, done } = normalizeIteratorInputs(bitmap, buffer));

    this.next = function next() {
      if (done) {
        result ||= new RoaringBitmap32IteratorResult();
        result.value = undefined;
        result.done = true;
        return result;
      }

      while (bufferIndex >= bufferCount) {
        if (reader === null) {
          if (!chunk) {
            chunk = acquireIteratorBuffer(bufferLength);
          }
          reader = new RoaringBitmap32BufferedIterator(bitmap, chunk, reverse);
          bufferCount = reader.n;
          bitmap = null;
        } else {
          bufferCount = reader.fill();
        }
        bufferIndex = 0;
        if (bufferCount <= 0) {
          if (reader) {
            reader.close();
            reader = null;
          }
          if (bufferReusable && chunk) {
            releaseIteratorBuffer(chunk);
          }
          bitmap = null;
          chunk = null;
          bufferReusable = false;
          done = true;
          result ||= new RoaringBitmap32IteratorResult();
          result.value = undefined;
          result.done = true;
          return result;
        }
      }

      result = result || new RoaringBitmap32IteratorResult();
      result.value = chunk[bufferIndex++];
      result.done = false;
      return result;
    };

    const iteratorReturn = function iteratorReturn(value) {
      result ||= new RoaringBitmap32IteratorResult();
      if (!done) {
        done = true;
        bitmap = null;
        if (reader) {
          reader.close();
          reader = null;
        }
        if (bufferReusable && chunk) {
          releaseIteratorBuffer(chunk);
        }
        chunk = null;
        bufferReusable = false;
      }
      result.value = value;
      result.done = true;
      return result;
    };

    this.return = iteratorReturn;
  }

  defineProperty(Iterator, "name", { value: name, configurable: true, enumerable: false, writable: false });

  Iterator.prototype[Symbol.iterator] = function () {
    return this;
  };

  Iterator.prototype.dispose = function dispose(value) {
    return this.return(value);
  };

  if (_symbolDispose) {
    Iterator.prototype[_symbolDispose] = Iterator.prototype.dispose;
  }

  defineProperty(Iterator, "default", {
    value: Iterator,
    writable: false,
    configurable: false,
    enumerable: true,
  });

  return Iterator;
}

const RoaringBitmap32Iterator = defineRoaringBitmap32Iterator(false, "RoaringBitmap32Iterator");

const RoaringBitmap32ReverseIterator = defineRoaringBitmap32Iterator(true, "RoaringBitmap32ReverseIterator");

function iterator() {
  return new RoaringBitmap32Iterator(this);
}

function reverseIterator() {
  return new RoaringBitmap32ReverseIterator(this);
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
  roaringBitmap32_proto.reverseIterator = reverseIterator;
  roaringBitmap32_proto.entries = function* entries() {
    for (const v of this) {
      yield [v, v];
    }
  };

  roaringBitmap32_proto.dispose = function dispose() {
    this.clear();
  };

  if (_symbolDispose) {
    roaringBitmap32_proto[_symbolDispose] = roaringBitmap32_proto.dispose;
  }

  roaringBitmap32_proto.forEach = function forEach(fn, self) {
    if (typeof fn !== "function") {
      throw new TypeError(`${fn} is not a function`);
    }
    let index = 0;
    if (self === undefined) {
      for (const v of this) {
        fn(v, index++, this);
      }
    } else {
      for (const v of this) {
        fn.call(self, v, index++, this);
      }
    }
    return this;
  };

  roaringBitmap32_proto.map = function map(fn, self, output) {
    if (typeof fn !== "function") {
      throw new TypeError(`${fn} is not a function`);
    }
    let index = 0;
    if (!output) {
      output = new Array(this.size);
      if (self === undefined) {
        for (const v of this) {
          output[index] = fn(v, index++, this);
        }
      } else {
        for (const v of this) {
          output[index] = fn.call(self, v, index++, this);
        }
      }
      return output;
    }
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

  roaringBitmap32_proto.every = function every(fn, self) {
    if (typeof fn !== "function") {
      throw new TypeError(`${fn} is not a function`);
    }
    let index = 0;
    if (self === undefined) {
      for (const v of this) {
        if (!fn(v, index++, this)) {
          return false;
        }
      }
    } else {
      for (const v of this) {
        if (!fn.call(self, v, index++, this)) {
          return false;
        }
      }
    }
    return true;
  };

  roaringBitmap32_proto.some = function some(fn, self) {
    if (typeof fn !== "function") {
      throw new TypeError(`${fn} is not a function`);
    }
    let index = 0;
    if (self === undefined) {
      for (const v of this) {
        if (fn(v, index++, this)) {
          return true;
        }
      }
    } else {
      for (const v of this) {
        if (fn.call(self, v, index++, this)) {
          return true;
        }
      }
    }
    return false;
  };

  roaringBitmap32_proto.reduce = function reduce(fn, initialValue = 0) {
    if (typeof fn !== "function") {
      throw new TypeError(`${fn} is not a function`);
    }
    let index = 0;
    let accumulator = initialValue;
    for (const v of this) {
      accumulator = fn(accumulator, v, index++, this);
    }
    return accumulator;
  };

  roaringBitmap32_proto.reduceRight = function reduceRight(fn, initialValue = 0) {
    if (typeof fn !== "function") {
      throw new TypeError(`${fn} is not a function`);
    }
    let index = this.size - 1;
    let accumulator = initialValue;
    for (const v of this.reverseIterator()) {
      accumulator = fn(accumulator, v, index--, this);
    }
    return accumulator;
  };

  roaringBitmap32_proto.find = function find(fn, self) {
    if (typeof fn !== "function") {
      throw new TypeError(`${fn} is not a function`);
    }
    let index = 0;
    if (self === undefined) {
      for (const v of this) {
        if (fn(v, index++, this)) {
          return v;
        }
      }
    } else {
      for (const v of this) {
        if (fn.call(self, v, index++, this)) {
          return v;
        }
      }
    }
    return undefined;
  };

  roaringBitmap32_proto.findIndex = function findIndex(fn, self) {
    if (typeof fn !== "function") {
      throw new TypeError(`${fn} is not a function`);
    }
    let index = 0;
    if (self === undefined) {
      for (const v of this) {
        if (fn(v, index, this)) {
          return index;
        }
        ++index;
      }
    } else {
      for (const v of this) {
        if (fn.call(self, v, index, this)) {
          return index;
        }
        ++index;
      }
    }
    return -1;
  };

  roaringBitmap32_proto.filter = function filter(fn, self, output = []) {
    if (typeof fn !== "function") {
      throw new TypeError(`${fn} is not a function`);
    }
    let index = 0;
    if (self === undefined) {
      for (const v of this) {
        if (fn(v, index++, this)) {
          output.push(v);
        }
      }
    } else {
      for (const v of this) {
        if (fn.call(self, v, index++, this)) {
          output.push(v);
        }
      }
    }
    return output;
  };

  roaringBitmap32_proto.union = function union(other) {
    if (other instanceof RoaringBitmap32) {
      return RoaringBitmap32.or(this, other);
    }
    const set = this.toSet();
    if (other) {
      for (const v of other.keys()) {
        set.add(v);
      }
    }
    return set;
  };

  roaringBitmap32_proto.intersection = function intersection(other) {
    if (other instanceof RoaringBitmap32) {
      return RoaringBitmap32.and(this, other);
    }
    let smaller;
    let larger;
    if (this.size <= other.size) {
      smaller = this;
      larger = other;
    } else {
      smaller = other.keys();
      larger = this;
    }
    const result = new Set();
    for (const elem of smaller) {
      if (larger.has(elem)) {
        result.add(elem);
      }
    }
    return result;
  };

  roaringBitmap32_proto.difference = function difference(other) {
    if (other instanceof RoaringBitmap32) {
      return RoaringBitmap32.andNot(this, other);
    }

    if (this.size <= other.size) {
      const result = this.toSet();
      for (const elem of this) {
        if (other.has(elem)) {
          result.delete(elem);
        }
      }
      return result;
    }

    const result = new Set();
    for (const v of this) {
      if (!other.has(v)) {
        result.add(v);
      }
    }
    return result;
  };

  roaringBitmap32_proto.symmetricDifference = function symmetricDifference(other) {
    if (other instanceof RoaringBitmap32) {
      return RoaringBitmap32.xor(this, other);
    }
    const result = this.toSet();
    for (const elem of other.keys()) {
      if (this.has(elem)) {
        result.delete(elem);
      } else {
        result.add(elem);
      }
    }
    return result;
  };

  roaringBitmap32_proto.isSupersetOf = function isSupersetOf(other) {
    if (other instanceof RoaringBitmap32) {
      return this.isSuperset(other);
    }
    for (const v of other.keys()) {
      if (!this.has(v)) {
        return false;
      }
    }
    return true;
  };

  roaringBitmap32_proto.isSubsetOf = function isSubsetOf(other) {
    if (other instanceof RoaringBitmap32) {
      return this.isSubset(other);
    }
    for (const v of this) {
      if (!other.has(v)) {
        return false;
      }
    }
    return true;
  };

  roaringBitmap32_proto.isDisjointFrom = function isDisjointFrom(other) {
    if (other instanceof RoaringBitmap32) {
      return this.andCardinality(other) === 0;
    }
    if (this.size <= other.size) {
      for (const elem of this) {
        if (other.has(elem)) {
          return false;
        }
      }
    } else {
      for (const elem of other.keys()) {
        if (this.has(elem)) {
          return false;
        }
      }
    }
    return true;
  };

  roaringBitmap32_proto.toSorted = function toSorted(cmp) {
    if (cmp && typeof cmp !== "function") {
      throw new TypeError(`${cmp} is not a function`);
    }
    const result = this.toArray();
    return cmp ? result.sort(cmp) : result;
  };

  roaringBitmap32_proto.toJSON = function toJSON() {
    return this.toArray();
  };

  const defineProp = (name, prop) => {
    defineProperty(roaring, name, prop);
    defineProperty(RoaringBitmap32, name, prop);
    defineProperty(roaringBitmap32_proto, name, prop);
  };

  const defineValue = (name, value, writable) =>
    defineProp(name, { value, writable: !!writable, configurable: false, enumerable: true });

  let packageVersion = null;

  defineProp("PackageVersion", {
    get: () => {
      if (packageVersion === null) {
        packageVersion = require("./package.json").version;
      }
      return packageVersion;
    },
    configurable: false,
    enumerable: true,
  });

  defineValue("RoaringBitmap32Iterator", RoaringBitmap32Iterator, false);
  defineValue("RoaringBitmap32ReverseIterator", RoaringBitmap32ReverseIterator, false);

  defineValue(
    "SerializationFormat",
    {
      croaring: "croaring",
      portable: "portable",
      unsafe_frozen_croaring: "unsafe_frozen_croaring",
      uint32_array: "uint32_array",
    },
    false,
  );

  defineValue(
    "FileSerializationFormat",
    {
      croaring: "croaring",
      portable: "portable",
      unsafe_frozen_croaring: "unsafe_frozen_croaring",
      uint32_array: "uint32_array",
      comma_separated_values: "comma_separated_values",
      tab_separated_values: "tab_separated_values",
      newline_separated_values: "newline_separated_values",
      json_array: "json_array",
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
      uint32_array: "uint32_array",
      comma_separated_values: "comma_separated_values",
      tab_separated_values: "tab_separated_values",
      newline_separated_values: "newline_separated_values",
      json_array: "json_array",
    },
    false,
  );

  defineValue(
    "FileDeserializationFormat",
    {
      croaring: "croaring",
      portable: "portable",
      unsafe_frozen_croaring: "unsafe_frozen_croaring",
      unsafe_frozen_portable: "unsafe_frozen_portable",
      uint32_array: "uint32_array",
      comma_separated_values: "comma_separated_values",
      tab_separated_values: "tab_separated_values",
      newline_separated_values: "newline_separated_values",
      json_array: "json_array",
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
