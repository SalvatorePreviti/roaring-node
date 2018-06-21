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
NodeJS   : v10.5.0 - V8 v6.7.288.46-node.8

* running 8 files...

• suite intersection (in place)
  65536 elements
  ✔ Set                  174.83 ops/sec  ±1.78%  61 runs  -99.97%
  ✔ FastBitSet        87,348.23 ops/sec  ±1.95%  81 runs  -87.09%
  ✔ RoaringBitmap32  676,713.04 ops/sec  ±1.55%  84 runs  fastest
  ➔ Fastest is RoaringBitmap32

• suite intersection (new)
  1048576 elements
  ✔ Set                  3.36 ops/sec   ±4.14%  13 runs  -99.87%
  ✔ FastBitSet       1,165.11 ops/sec   ±1.43%  78 runs  -53.56%
  ✔ RoaringBitmap32  2,508.67 ops/sec  ±22.68%  44 runs  fastest
  ➔ Fastest is RoaringBitmap32

• suite add
  65535 elements
  ✔ Set.add                                370.50 ops/sec  ±1.19%  75 runs  -82.46%
  ✔ RoaringBitmap32.add                    497.47 ops/sec  ±2.08%  79 runs  -76.45%
  ✔ RoaringBitmap32.addMany Array        1,518.58 ops/sec  ±1.80%  78 runs  -28.11%
  ✔ RoaringBitmap32.addMany Uint32Array  2,112.26 ops/sec  ±3.32%  71 runs  fastest
  ➔ Fastest is RoaringBitmap32.addMany Uint32Array

• suite iterator
  65536 elements
  ✔ Set              1,365.34 ops/sec  ±2.37%  82 runs  fastest
  ✔ RoaringBitmap32    754.09 ops/sec  ±2.47%  78 runs  -44.77%
  ➔ Fastest is Set

• suite intersection size
  262144 elements
  ✔ Set                   23.68 ops/sec  ±5.57%  42 runs  -99.99%
  ✔ FastBitSet        14,501.31 ops/sec  ±0.98%  83 runs  -94.10%
  ✔ RoaringBitmap32  245,786.86 ops/sec  ±2.21%  82 runs  fastest
  ➔ Fastest is RoaringBitmap32

• suite union (in place)
  65536 elements
  ✔ Set                  228.73 ops/sec  ±3.27%  63 runs  -99.97%
  ✔ FastBitSet       153,633.79 ops/sec  ±2.79%  80 runs  -80.39%
  ✔ RoaringBitmap32  783,632.01 ops/sec  ±0.63%  89 runs  fastest
  ➔ Fastest is RoaringBitmap32

• suite union size
  262144 elements
  ✔ Set                   17.87 ops/sec  ±2.03%  47 runs  -99.99%
  ✔ FastBitSet         7,699.76 ops/sec  ±2.28%  83 runs  -97.03%
  ✔ RoaringBitmap32  258,921.56 ops/sec  ±1.29%  86 runs  fastest
  ➔ Fastest is RoaringBitmap32

• suite union (new)
  1048576 elements
  ✔ Set                  1.78 ops/sec   ±3.62%   9 runs  -99.90%
  ✔ FastBitSet         682.05 ops/sec   ±1.17%  83 runs  -60.95%
  ✔ RoaringBitmap32  1,746.45 ops/sec  ±15.85%  46 runs  fastest
  ➔ Fastest is RoaringBitmap32


* completed: 54273.887ms
```
