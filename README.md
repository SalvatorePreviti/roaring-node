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

- This package - <https://www.npmjs.com/package/roaring>
- Source code and build tools for this package - <https://github.com/SalvatorePreviti/roaring-node>
- Roaring Bitmaps - <http://roaringbitmap.org/>
- Portable Roaring bitmaps in C - <https://github.com/RoaringBitmap/CRoaring>
- Portable Roaring bitmaps in C (unity build) - https://github.com/lemire/CRoaringUnityBuild

# Licenses

- This package is provided as open source software using Apache License.
- CRoaring is provided as open source software using Apache License.

# API

See the [API documentation](https://github.com/SalvatorePreviti/roaring-node/blob/master/documentation/api/index.d.md)

See the [RoaringBitmap32 class documentation](https://github.com/SalvatorePreviti/roaring-node/blob/master/documentation/api/index.d/roaringbitmap32.md#roaringbitmap32)

- To disable AVX2 instruction set, set the environment variable ROARING_DISABLE_AVX2 to 'true' before requiring this package.
- To disable SSE42 instruction set, set the environment variable ROARING_DISABLE_SSE42 to 'true' before requiring this package.

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

After a full recomple, is possible to compile only changed files:

```
npm run compile
```

### To run the unit test

```
npm test
```

### To regenerate the API documentation

```
npm run doc
```

### To run the performance benchmarks

```sh
npm run benchmarks
```

It will produce a result similar to this one:

```
Platform : Darwin 17.6.0 x64
CPU      : Intel(R) Core(TM) i7-7700HQ CPU @ 2.80GHz AVX2
Cores    : 4 physical - 8 logical
Memory   : 16.00 GB
NodeJS   : v10.5.0 - V8 v6.7.288.46-node.8

* running 8 files...

• suite intersection (in place)
  65536 elements
  ✔ Set                  175.80 ops/sec  ±4.53%  65 runs  -99.98%
  ✔ FastBitSet        97,245.74 ops/sec  ±2.68%  84 runs  -86.38%
  ✔ RoaringBitmap32  714,009.57 ops/sec  ±3.33%  82 runs  fastest
  ➔ Fastest is RoaringBitmap32

• suite intersection (new)
  1048576 elements
  ✔ Set                  3.45 ops/sec   ±6.51%  13 runs  -99.84%
  ✔ FastBitSet       1,322.72 ops/sec   ±1.13%  80 runs  -39.01%
  ✔ RoaringBitmap32  2,168.88 ops/sec  ±18.13%  28 runs  fastest
  ➔ Fastest is RoaringBitmap32

• suite add
  65535 elements
  ✔ Set.add                                368.42 ops/sec  ±2.80%  74 runs  -85.49%
  ✔ RoaringBitmap32.tryAdd                 509.60 ops/sec  ±2.96%  84 runs  -79.92%
  ✔ RoaringBitmap32.add                    521.23 ops/sec  ±3.52%  79 runs  -79.47%
  ✔ RoaringBitmap32.addMany Array        1,266.89 ops/sec  ±4.03%  79 runs  -50.09%
  ✔ RoaringBitmap32.addMany Uint32Array  2,538.37 ops/sec  ±3.68%  79 runs  fastest
  ➔ Fastest is RoaringBitmap32.addMany Uint32Array

• suite iterator
  65536 elements
  ✔ Set              1,512.74 ops/sec  ±2.01%  84 runs  fastest
  ✔ RoaringBitmap32  1,150.85 ops/sec  ±2.91%  84 runs  -23.92%
  ➔ Fastest is Set

• suite intersection size
  262144 elements
  ✔ Set                   26.43 ops/sec  ±7.18%  48 runs  -99.99%
  ✔ FastBitSet        15,046.39 ops/sec  ±2.46%  85 runs  -94.33%
  ✔ RoaringBitmap32  265,435.07 ops/sec  ±2.46%  84 runs  fastest
  ➔ Fastest is RoaringBitmap32

• suite union (in place)
  65536 elements
  ✔ Set                  304.50 ops/sec  ±2.66%  67 runs  -99.97%
  ✔ FastBitSet       149,467.73 ops/sec  ±1.18%  73 runs  -83.36%
  ✔ RoaringBitmap32  898,339.76 ops/sec  ±2.26%  84 runs  fastest
  ➔ Fastest is RoaringBitmap32

• suite union size
  262144 elements
  ✔ Set                   18.56 ops/sec  ±1.87%  37 runs  -99.99%
  ✔ FastBitSet         8,286.87 ops/sec  ±1.52%  87 runs  -97.18%
  ✔ RoaringBitmap32  294,325.10 ops/sec  ±2.19%  85 runs  fastest
  ➔ Fastest is RoaringBitmap32

• suite union (new)
  1048576 elements
  ✔ Set                  1.86 ops/sec   ±8.48%   9 runs  -99.90%
  ✔ FastBitSet         717.62 ops/sec   ±1.48%  86 runs  -61.25%
  ✔ RoaringBitmap32  1,852.03 ops/sec  ±13.50%  48 runs  fastest
  ➔ Fastest is RoaringBitmap32

* completed: 54213.010ms
```
