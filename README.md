# roaring

Port of [Roaring Bitmaps](http://roaringbitmap.org) for NodeJS as a native addon.

It is interoperable with other implementations via the [Roaring format](https://github.com/RoaringBitmap/RoaringFormatSpec/).
It takes advantage of AVX2 or SSE4.2 instructions on 64 bit platforms that supports it.

Roaring bitmaps are compressed bitmaps. They can be hundreds of times faster.

For a precompiled binary of this package compatible with AWS Lambda NodeJS v8.10.0, use [roaring-aws](https://www.npmjs.com/package/roaring-aws).

## Installation

```sh
npm install --save roaring
```

## References

This package - <https://www.npmjs.com/package/roaring>

Source code and build tools for this package - <https://github.com/SalvatorePreviti/roaring-node>

Roaring Bitmaps - <http://roaringbitmap.org/>

Portable Roaring bitmaps in C - <https://github.com/RoaringBitmap/CRoaring>

Portable Roaring bitmaps in C (unity build) - https://github.com/lemire/CRoaringUnityBuild

# Licenses

- This package is provided as open source software using Apache License.

- CRoaring is provided as open source software using Apache License.

# Code sample:

```javascript
// npm install --save roaring
// create this file as demo.js
// type node demo.js

const RoaringBitmap32 = require('roaring/RoaringBitmap32')

const bitmap1 = new RoaringBitmap32([1, 2, 3, 4, 5])
bitmap1.addMany([100, 1000])
console.log('bitmap1.toArray():', bitmap1.toArray())

const bitmap2 = new RoaringBitmap32([3, 4, 1000])
console.log('bitmap2.toArray():', bitmap2.toArray())

const bitmap3 = new RoaringBitmap32()
console.log('bitmap1.size:', bitmap1.size)
console.log('bitmap3.has(3):', bitmap3.has(3))
bitmap3.add(3)
console.log('bitmap3.has(3):', bitmap3.has(3))

bitmap3.add(111)
bitmap3.add(544)
bitmap3.orInPlace(bitmap1)
bitmap1.runOptimize()
bitmap1.shrinkToFit()
console.log('contentToString:', bitmap3.contentToString())

console.log('bitmap3.toArray():', bitmap3.toArray())
console.log('bitmap3.maximum():', bitmap3.maximum())
console.log('bitmap3.rank(100):', bitmap3.rank(100))

const iterated = []
for (const value of bitmap3) {
  iterated.push(value)
}
console.log('iterated:', iterated)

const serialized = bitmap3.serialize()
console.log('serialized:', serialized.toString('base64'))
console.log('deserialized:', RoaringBitmap32.deserialize(serialized).toArray())
```

# API

See the .d.ts declaration files and check the source code at <https://github.com/SalvatorePreviti/roaring-node>

To disable AVX2 instruction set, set the environment variable ROARING_DISABLE_AVX2 to 'true' before requiring this package.

To disable SSE42 instruction set, set the environment variable ROARING_DISABLE_SSE42 to 'true' before requiring this package.

# Development, local building

Clone the repository and install all the dependencies

```
git clone https://github.com/SalvatorePreviti/roaring-node.git

git submodule update --init --recursive

npm install
```

### To rebuild the C++ sources

```
npm run recompile
```

### To run the unit test

```
npm test
```

### To run the performance benchmarks

```sh
npm run benchmarks
```

It will produce a result similar to this one

```
Platform : Darwin 17.6.0 x64
CPU      : Intel(R) Core(TM) i7-7700HQ CPU @ 2.80GHz AVX2
Cores    : 4 physical - 8 logical
Memory   : 16.00 GB
NodeJS   : v10.4.1 - V8 v6.7.288.45-node.7

* running 7 files...

• suite intersection (in place)
  65536 elements
  ✔ Set                  168.20 ops/sec  ±1.29%  67 runs  -99.98%
  ✔ FastBitSet        89,719.96 ops/sec  ±2.36%  78 runs  -88.26%
  ✔ RoaringBitmap32  764,250.04 ops/sec  ±1.06%  84 runs  fastest
  ➔ Fastest is RoaringBitmap32

• suite intersection size
  262144 elements
  ✔ Set                   28.73 ops/sec  ±1.47%  50 runs  -99.99%
  ✔ FastBitSet        13,905.61 ops/sec  ±3.44%  79 runs  -94.66%
  ✔ RoaringBitmap32  260,501.43 ops/sec  ±1.15%  89 runs  fastest
  ➔ Fastest is RoaringBitmap32

• suite intersection (new)
  1048576 elements
  ✔ Set                  3.30 ops/sec   ±5.70%  13 runs  -99.89%
  ✔ FastBitSet       1,271.71 ops/sec   ±1.64%  85 runs  -57.86%
  ✔ RoaringBitmap32  3,018.00 ops/sec  ±17.13%  48 runs  fastest
  ➔ Fastest is RoaringBitmap32

• suite iterator
  65536 elements
  ✔ Set              1,330.50 ops/sec  ±4.65%  75 runs  fastest
  ✔ RoaringBitmap32    209.38 ops/sec  ±1.44%  77 runs  -84.26%
  ➔ Fastest is Set

• suite union (in place)
  65536 elements
  ✔ Set                  177.77 ops/sec  ±9.24%  55 runs  -99.98%
  ✔ FastBitSet       164,512.21 ops/sec  ±1.51%  82 runs  -78.34%
  ✔ RoaringBitmap32  759,533.50 ops/sec  ±2.22%  77 runs  fastest
  ➔ Fastest is RoaringBitmap32

• suite union (new)
  1048576 elements
  ✔ Set                  1.94 ops/sec  ±11.54%   9 runs  -99.88%
  ✔ FastBitSet         680.29 ops/sec   ±2.69%  82 runs  -56.13%
  ✔ RoaringBitmap32  1,550.53 ops/sec  ±19.60%  47 runs  fastest
  ➔ Fastest is RoaringBitmap32

• suite union size
  262144 elements
  ✔ Set                   16.05 ops/sec  ±2.75%  43 runs  -99.99%
  ✔ FastBitSet         8,387.83 ops/sec  ±2.48%  86 runs  -96.47%
  ✔ RoaringBitmap32  237,543.69 ops/sec  ±4.90%  75 runs  fastest
  ➔ Fastest is RoaringBitmap32


* completed: 44249.259ms

```
