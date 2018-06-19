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

* running 6 files...

• suite intersection size
  262144 elements
  ✔ Set                   33.26 ops/sec  ±1.06%  57 runs  -99.99%
  ✔ FastBitSet        14,364.56 ops/sec  ±3.95%  83 runs  -94.61%
  ✔ RoaringBitmap32  266,718.85 ops/sec  ±1.01%  86 runs  fastest
  ➔ Fastest is RoaringBitmap32

• suite intersection (in place)
  65536 elements
  ✔ Set                    199.99 ops/sec  ±1.96%  63 runs  -100.00%
  ✔ FastBitSet          93,394.64 ops/sec  ±3.38%  79 runs   -98.02%
  ✔ RoaringBitmap32  4,720,764.58 ops/sec  ±0.81%  87 runs  fastest
  ➔ Fastest is RoaringBitmap32

• suite intersection (new)
  1048576 elements
  ✔ Set                  3.32 ops/sec  ±6.64%  13 runs  -99.91%
  ✔ FastBitSet       1,436.14 ops/sec  ±1.34%  85 runs  -59.63%
  ✔ RoaringBitmap32  3,557.16 ops/sec  ±4.49%  51 runs  fastest
  ➔ Fastest is RoaringBitmap32

• suite union (in place)
  65536 elements
  ✔ Set                  201.71 ops/sec  ±3.45%  61 runs  -99.96%
  ✔ FastBitSet       147,147.28 ops/sec  ±1.18%  80 runs  -70.43%
  ✔ RoaringBitmap32  497,687.77 ops/sec  ±2.84%  83 runs  fastest
  ➔ Fastest is RoaringBitmap32

• suite union size
  262144 elements
  ✔ Set                   22.77 ops/sec  ±2.08%  42 runs  -99.99%
  ✔ FastBitSet         7,766.65 ops/sec  ±2.62%  83 runs  -97.17%
  ✔ RoaringBitmap32  274,167.71 ops/sec  ±1.12%  86 runs  fastest
  ➔ Fastest is RoaringBitmap32

• suite union (new)
  1048576 elements
  ✔ Set                  1.72 ops/sec   ±4.90%   9 runs  -99.92%
  ✔ FastBitSet         698.26 ops/sec   ±1.89%  76 runs  -65.66%
  ✔ RoaringBitmap32  2,033.11 ops/sec  ±14.20%  49 runs  fastest
  ➔ Fastest is RoaringBitmap32


* completed: 39997.923ms
```
