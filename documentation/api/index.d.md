# Table of contents

* [index.d.ts][SourceFile-0]
    * Variables
        * [instructionSet][VariableDeclaration-0]
        * [SSE42][VariableDeclaration-1]
        * [AVX2][VariableDeclaration-2]
        * [CRoaringVersion][VariableDeclaration-3]
        * [PackageVersion][VariableDeclaration-4]

# index.d.ts

## Classes

### [RoaringBitmap32][ClassDeclaration-0]

# roaring

Port of [Roaring Bitmaps](http://roaringbitmap.org) for NodeJS as a native addon.

It is interoperable with other implementations via the [Roaring format](https://github.com/RoaringBitmap/RoaringFormatSpec/).

Roaring bitmaps are compressed bitmaps. They can be hundreds of times faster.

For a precompiled binary of this package compatible with AWS Lambda NodeJS v8.10.0, use [roaring-aws](https://www.npmjs.com/package/roaring-aws).

It takes advantage of AVX2 or SSE4.2 instructions on 64 bit platforms that supports it.
- To disable AVX2 instruction set, set the environment variable ROARING_DISABLE_AVX2 to 'true' before requiring this package.
- To disable SSE42 instruction set, set the environment variable ROARING_DISABLE_SSE42 to 'true' before requiring this package.

## Installation

`npm install --save roaring`

## References

- This package - <https://www.npmjs.com/package/roaring>
- Source code and build tools for this package - <https://github.com/SalvatorePreviti/roaring-node>
- Roaring Bitmaps - <http://roaringbitmap.org/>
- Portable Roaring bitmaps in C - <https://github.com/RoaringBitmap/CRoaring>
- Portable Roaring bitmaps in C (unity build) - https://github.com/lemire/CRoaringUnityBuild

## Licenses

- This package is provided as open source software using Apache License.
- CRoaring is provided as open source software using Apache License.

## Example:

`const RoaringBitmap32 = require('roaring/RoaringBitmap32')`
`const bitmap = new RoaringBitmap32([1, 2, 3])`
`bitmap.add(9)`
`console.log(bitmap.contentToString())`
Roaring bitmap that supports 32 bit unsigned integers.

See http://roaringbitmap.org/
See https://github.com/SalvatorePreviti/roaring-node


----------

### [RoaringBitmap32Iterator][ClassDeclaration-1]

Iterator for RoaringBitmap32


## Variables

### instructionSet

Property: The instruction set supported and currently used by the underlying CRoraring library.
Possible values are:
  - 'AVX2' - Advanced Vector Extensions 2
  - 'SSE42' - Streaming SIMD Extensions 4.2
  - 'PLAIN' - no special instruction set

```typescript
var instructionSet: "AVX2" | "SSE42" | "PLAIN";
```

**Type**

"AVX2" | "SSE42" | "PLAIN"

----------

### SSE42

Property: Indicates wether Streaming SIMD Extensions 4.2 instruction set is supported and currently used by the underlying CRoaring library.

```typescript
var SSE42: boolean;
```

**Type**

boolean

----------

### AVX2

Property: Indicates wether Advanced Vector Extensions 2 instruction set is supported and currently used by the underlying CRoaring library.

```typescript
var AVX2: boolean;
```

**Type**

boolean

----------

### CRoaringVersion

Property: The version of the CRoaring libary as a string.
Example: "0.2.42"

```typescript
var CRoaringVersion: string;
```

**Type**

string

----------

### PackageVersion

Property: The version of the roaring npm package as a string.
Example: "0.2.2"

```typescript
var PackageVersion: string;
```

**Type**

string

[SourceFile-0]: index.d.md#indexdts
[ClassDeclaration-0]: index.d/roaringbitmap32.md#roaringbitmap32
[ClassDeclaration-1]: index.d/roaringbitmap32iterator.md#roaringbitmap32iterator
[VariableDeclaration-0]: index.d.md#instructionset
[VariableDeclaration-1]: index.d.md#sse42
[VariableDeclaration-2]: index.d.md#avx2
[VariableDeclaration-3]: index.d.md#croaringversion
[VariableDeclaration-4]: index.d.md#packageversion