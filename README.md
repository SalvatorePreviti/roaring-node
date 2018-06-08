# roaring

Port of [Roaring Bitmaps](http://roaringbitmap.org) for NodeJS as a native addon.
It is interoperable with other implementations via the [Roaring format](https://github.com/RoaringBitmap/RoaringFormatSpec/).

Roaring bitmaps are compressed bitmaps. They can be hundreds of times faster.

For a precompiled binary of this package compatible with AWS Lambda NodeJS v8.10.0, use [roaring-aws](https://www.npmjs.com/package/roaring).

## installation

```sh
npm install --save roaring
```

## references

This package - <https://www.npmjs.com/package/roaring>

Source code and build tools for this package - <https://github.com/SalvatorePreviti/roaring-node>

Roaring Bitmaps - <http://roaringbitmap.org/>

Portable Roaring bitmaps in C - <https://github.com/RoaringBitmap/CRoaring>

# licenses

- This package is provided as open source software using Apache License.

- CRoaring is provided as open source software using Apache License.

# API

See the .d.ts declaration files

# Development, local building

To rebuild the C++ sources

```
git submodule update --init --recursive

npm run recomple
```

To run the unit test

```
npm test
```
