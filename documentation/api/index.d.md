# Table of contents

* [index.d.ts][SourceFile-0]
    * Types
        * [RoaringBitmap32Callback][TypeAliasDeclaration-0]
    * Variables
        * [instructionSet][VariableDeclaration-0]
        * [SSE42][VariableDeclaration-1]
        * [AVX2][VariableDeclaration-2]
        * [CRoaringVersion][VariableDeclaration-3]
        * [PackageVersion][VariableDeclaration-4]

# index.d.ts

## Types

### RoaringBitmap32Callback

```typescript
type RoaringBitmap32Callback = (error: Error | null, bitmap: RoaringBitmap32 | undefined) => void;
```

**Type**

(error: Error | null, bitmap: RoaringBitmap32 | undefined) => void

## Classes

### [RoaringBitmap32][ClassDeclaration-0]

Roaring bitmap that supports 32 bit unsigned integers.

- See http://roaringbitmap.org/
- See https://github.com/SalvatorePreviti/roaring-node


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
[TypeAliasDeclaration-0]: index.d.md#roaringbitmap32callback
[ClassDeclaration-0]: index.d/roaringbitmap32.md#roaringbitmap32
[ClassDeclaration-1]: index.d/roaringbitmap32iterator.md#roaringbitmap32iterator
[VariableDeclaration-0]: index.d.md#instructionset
[VariableDeclaration-1]: index.d.md#sse42
[VariableDeclaration-2]: index.d.md#avx2
[VariableDeclaration-3]: index.d.md#croaringversion
[VariableDeclaration-4]: index.d.md#packageversion