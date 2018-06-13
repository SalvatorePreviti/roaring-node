# roaring

Port of [Roaring Bitmaps](http://roaringbitmap.org) for NodeJS as a native addon.
It is interoperable with other implementations via the [Roaring format](https://github.com/RoaringBitmap/RoaringFormatSpec/).

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

# Licenses

- This package is provided as open source software using Apache License.

- CRoaring is provided as open source software using Apache License.

# Code sample:

```javascript
// npm install --save roaring
// create this file as demo.js
// type node demo.js

var RoaringBitmap32 = require('roaring/RoaringBitmap32')
const RoaringBitmap32 = require('./RoaringBitmap32')

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

See the .d.ts declaration files

# Development, local building

To rebuild the C++ sources

```
git submodule update --init --recursive

npm run recompile
```

To run the unit test

```
npm test
```
