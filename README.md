# roaring

Official port of [Roaring Bitmaps](http://roaringbitmap.org) for NodeJS as a native addon.

It is interoperable with other implementations via the [Roaring format](https://github.com/RoaringBitmap/RoaringFormatSpec/).
It takes advantage of AVX2 or SSE4.2 instructions on 64 bit platforms that supports it.

Roaring bitmaps are compressed bitmaps. They can be hundreds of times faster.

For a precompiled binary of this package compatible with AWS Lambda NodeJS v8.10.0, use [roaring-aws](https://www.npmjs.com/package/roaring-aws).

## Branches

Branch `publish` contains the latest published stable version.

Branch `master` is the development branch that may contain code not yet published or ready for production.
If you want to contribute and submit a pull request, use the master branch.

## Supported node versions

Node 12.13+, 14.13+, 16.14+, 18+, 20+ are currently supported.

Node 8 and 10 support was dropped in release 2.0

## Worker thread support

IMPORTANT: You need Node >= 14 to for roaring-node to work with worker threads correctly. Previous version of node are not supported and will throw an error if this library is loaded inside a worker thread.

Directly transferring an instance without copy between worker threads is not currently supported, but you can create a frozen view on a SharedArrayBuffer using bufferAlignedAllocShared and pass it to the worker thread.

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

See the [roaring module documentation](https://salvatorepreviti.github.io/roaring-node/modules.html)

See the [RoaringBitmap32 class documentation](https://salvatorepreviti.github.io/roaring-node/classes/RoaringBitmap32.html)

# Code sample:

```javascript
// npm install --save roaring
// create this file as demo.js
// type node demo.js

const RoaringBitmap32 = require("roaring/RoaringBitmap32");

const bitmap1 = new RoaringBitmap32([1, 2, 3, 4, 5]);
bitmap1.addMany([100, 1000]);
console.log("bitmap1.toArray():", bitmap1.toArray());

const bitmap2 = new RoaringBitmap32([3, 4, 1000]);
console.log("bitmap2.toArray():", bitmap2.toArray());

const bitmap3 = new RoaringBitmap32();
console.log("bitmap1.size:", bitmap1.size);
console.log("bitmap3.has(3):", bitmap3.has(3));
bitmap3.add(3);
console.log("bitmap3.has(3):", bitmap3.has(3));

bitmap3.add(111);
bitmap3.add(544);
bitmap3.orInPlace(bitmap1);
bitmap1.runOptimize();
bitmap1.shrinkToFit();
console.log("contentToString:", bitmap3.contentToString());

console.log("bitmap3.toArray():", bitmap3.toArray());
console.log("bitmap3.maximum():", bitmap3.maximum());
console.log("bitmap3.rank(100):", bitmap3.rank(100));

const iterated = [];
for (const value of bitmap3) {
  iterated.push(value);
}
console.log("iterated:", iterated);

const serialized = bitmap3.serialize(false);
console.log("serialized:", serialized.toString("base64"));
console.log("deserialized:", RoaringBitmap32.deserialize(serialized, false).toArray());
```

# Other

Wanna play an open source game made by the author of this library? Try [Dante](https://github.com/SalvatorePreviti/js13k-2022)

# Development, local building

Clone the repository and install all the dependencies

```
git clone https://github.com/SalvatorePreviti/roaring-node.git

cd roaring-node

npm install
```

# To rebuild the roaring unity build updating to the latest version

```
./scripts/update-roaring.sh

npm run rebuild

```

### To rebuild the C++ sources

```
npm run rebuild
```

After a full recomple, is possible to compile only changed files:

```
npm run build
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
  ✔ Set                  186.82 ops/sec  ±2.33%  66 runs  -99.98%
  ✔ FastBitSet       100,341.63 ops/sec  ±2.10%  85 runs  -87.10%
  ✔ RoaringBitmap32  777,765.97 ops/sec  ±2.14%  87 runs  fastest
  ➔ Fastest is RoaringBitmap32

• suite intersection (new)
  1048576 elements
  ✔ Set                  3.49 ops/sec   ±3.82%  13 runs  -99.89%
  ✔ FastBitSet       1,463.87 ops/sec   ±1.13%  84 runs  -55.54%
  ✔ RoaringBitmap32  3,292.51 ops/sec  ±20.89%  44 runs  fastest
  ➔ Fastest is RoaringBitmap32

• suite add
  65535 elements
  ✔ Set.add                                438.37 ops/sec  ±4.48%  68 runs  -85.21%
  ✔ RoaringBitmap32.tryAdd                 489.70 ops/sec  ±3.19%  76 runs  -83.48%
  ✔ RoaringBitmap32.add                    528.09 ops/sec  ±3.38%  76 runs  -82.18%
  ✔ RoaringBitmap32.addMany Array        1,652.62 ops/sec  ±4.20%  64 runs  -44.25%
  ✔ RoaringBitmap32.addMany Uint32Array  2,964.19 ops/sec  ±1.47%  86 runs  fastest
  ➔ Fastest is RoaringBitmap32.addMany Uint32Array

• suite iterator
  65536 elements
  ✔ Set              1,648.55 ops/sec  ±1.21%  86 runs  fastest
  ✔ RoaringBitmap32  1,239.06 ops/sec  ±1.62%  86 runs  -24.84%
  ➔ Fastest is Set

• suite intersection size
  262144 elements
  ✔ Set                   29.10 ops/sec  ±5.65%  51 runs  -99.99%
  ✔ FastBitSet        15,938.45 ops/sec  ±2.01%  86 runs  -94.09%
  ✔ RoaringBitmap32  269,502.51 ops/sec  ±2.08%  84 runs  fastest
  ➔ Fastest is RoaringBitmap32

• suite union (in place)
  65536 elements
  ✔ Set                  298.53 ops/sec  ±3.07%  65 runs  -99.97%
  ✔ FastBitSet       144,914.65 ops/sec  ±2.10%  75 runs  -83.38%
  ✔ RoaringBitmap32  871,794.31 ops/sec  ±3.85%  82 runs  fastest
  ➔ Fastest is RoaringBitmap32

• suite union size
  262144 elements
  ✔ Set                   17.69 ops/sec  ±3.40%  33 runs  -99.99%
  ✔ FastBitSet         8,481.07 ops/sec  ±1.54%  84 runs  -97.02%
  ✔ RoaringBitmap32  284,749.52 ops/sec  ±1.35%  86 runs  fastest
  ➔ Fastest is RoaringBitmap32

• suite union (new)
  1048576 elements
  ✔ Set                  1.83 ops/sec   ±6.60%   9 runs  -99.90%
  ✔ FastBitSet         744.68 ops/sec   ±1.11%  85 runs  -60.45%
  ✔ RoaringBitmap32  1,882.93 ops/sec  ±16.32%  44 runs  fastest
  ➔ Fastest is RoaringBitmap32


* completed: 53610.279ms
```

Works also on M1

````
Platform : Darwin 21.1.0 arm64
CPU      : Apple M1 Pro
Cores    : 10 physical - 10 logical
Memory   : 16.00 GB
NodeJS   : v16.13.1 - V8 v9.4.146.24-node.14

* running 8 files...

• suite union (in place)
  65536 elements
  ✔ Set                    456.60 ops/sec  ±1.88%  77 runs  -99.97%
  ✔ FastBitSet         232,794.03 ops/sec  ±0.94%  91 runs  -86.52%
  ✔ RoaringBitmap32  1,727,310.24 ops/sec  ±1.12%  95 runs  fastest
  ➔ Fastest is RoaringBitmap32

• suite intersection size
  262144 elements
  ✔ Set                   71.20 ops/sec  ±2.30%  62 runs  -99.98%
  ✔ FastBitSet        21,525.29 ops/sec  ±0.71%  97 runs  -93.72%
  ✔ RoaringBitmap32  342,892.37 ops/sec  ±0.93%  95 runs  fastest
  ➔ Fastest is RoaringBitmap32

• suite intersection (in place)
  65536 elements
  ✔ Set                    280.62 ops/sec  ±1.66%  76 runs  -99.97%
  ✔ FastBitSet         136,148.03 ops/sec  ±0.56%  96 runs  -87.11%
  ✔ RoaringBitmap32  1,055,978.14 ops/sec  ±1.16%  92 runs  fastest
  ➔ Fastest is RoaringBitmap32

• suite union size
  262144 elements
  ✔ Set                   37.95 ops/sec  ±2.33%  51 runs  -99.99%
  ✔ FastBitSet        12,111.37 ops/sec  ±0.67%  96 runs  -96.35%
  ✔ RoaringBitmap32  331,510.04 ops/sec  ±1.02%  95 runs  fastest
  ➔ Fastest is RoaringBitmap32

• suite intersection (new)
  1048576 elements
  ✔ Set                  6.54 ops/sec  ±5.41%  21 runs  -99.93%
  ✔ FastBitSet       2,087.94 ops/sec  ±5.23%  42 runs  -78.88%
  ✔ RoaringBitmap32  9,888.24 ops/sec  ±1.77%  51 runs  fastest
  ➔ Fastest is RoaringBitmap32

• suite union (new)
  1048576 elements
  ✔ Set                  3.80 ops/sec  ±7.91%  14 runs  -99.93%
  ✔ FastBitSet       1,693.77 ops/sec  ±3.90%  64 runs  -70.63%
  ✔ RoaringBitmap32  5,767.35 ops/sec  ±1.74%  51 runs  fastest
  ➔ Fastest is RoaringBitmap32

• suite iterator
  65536 elements
  ✔ Set.iterator              10,033.75 ops/sec  ±1.40%  92 runs  fastest
  ✔ Set.forEach                1,595.08 ops/sec  ±2.02%  89 runs  -84.10%
  ✔ RoaringBitmap32.iterator   2,879.57 ops/sec  ±1.06%  93 runs  -71.30%
  ✔ RoaringBitmap32.forEach    1,764.98 ops/sec  ±0.68%  97 runs  -82.41%
  ➔ Fastest is Set.iterator

• suite add
  65535 elements
  ✔ Set.add                                508.76 ops/sec  ±1.27%   86 runs  -91.37%
  ✔ RoaringBitmap32.tryAdd                 707.70 ops/sec  ±0.81%   96 runs  -87.99%
  ✔ RoaringBitmap32.add                    699.27 ops/sec  ±1.08%   90 runs  -88.13%
  ✔ RoaringBitmap32.addMany Array        4,457.70 ops/sec  ±0.54%   90 runs  -24.35%
  ✔ RoaringBitmap32.addMany Uint32Array  5,892.57 ops/sec  ±0.07%  101 runs  fastest
  ➔ Fastest is RoaringBitmap32.addMany Uint32Array


* completed: 27.170s```

````
