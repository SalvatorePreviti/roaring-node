# Table of contents

* [index.d.ts][SourceFile-0]
    * Interfaces
        * [RoaringBitmap32Statistics][InterfaceDeclaration-1]
    * Types
        * [RoaringBitmap32Callback][TypeAliasDeclaration-0]
        * [RoaringBitmap32ArrayCallback][TypeAliasDeclaration-1]
    * Variables
        * [instructionSet][VariableDeclaration-0]
        * [SSE42][VariableDeclaration-1]
        * [AVX2][VariableDeclaration-2]
        * [CRoaringVersion][VariableDeclaration-3]
        * [PackageVersion][VariableDeclaration-4]

# index.d.ts

## Interfaces

### RoaringBitmap32Statistics

Object returned by RoaringBitmap32 statistics() method

```typescript
interface RoaringBitmap32Statistics {
    containers: number;
    arrayContainers: number;
    runContainers: number;
    bitsetContainers: number;
    valuesInArrayContainers: number;
    valuesInRunContainers: number;
    valuesInBitsetContainers: number;
    bytesInArrayContainers: number;
    bytesInRunContainers: number;
    bytesInBitsetContainers: number;
    maxValue: number;
    minValue: number;
    sumOfAllValues: number;
    size: number;
}
```

**Properties**

| Name                     | Type   | Optional | Description                                     |
| ------------------------ | ------ | -------- | ----------------------------------------------- |
| containers               | number | false    | Number of containers.                           |
| arrayContainers          | number | false    | Number of array containers.                     |
| runContainers            | number | false    | Number of run containers.                       |
| bitsetContainers         | number | false    | Number of bitmap containers.                    |
| valuesInArrayContainers  | number | false    | Number of values in array containers.           |
| valuesInRunContainers    | number | false    | Number of values in run containers.             |
| valuesInBitsetContainers | number | false    | Number of values in  bitmap containers.         |
| bytesInArrayContainers   | number | false    | Number of allocated bytes in array containers.  |
| bytesInRunContainers     | number | false    | Number of allocated bytes in run containers.    |
| bytesInBitsetContainers  | number | false    | Number of allocated bytes in bitmap containers. |
| maxValue                 | number | false    | The maximal value.                              |
| minValue                 | number | false    | The minimal value.                              |
| sumOfAllValues           | number | false    | The sum of all values                           |
| size                     | number | false    | Total number of values stored in the bitmap     |

## Types

### RoaringBitmap32Callback

```typescript
type RoaringBitmap32Callback = (error: Error | null, bitmap: RoaringBitmap32 | undefined) => void;
```

**Type**

(error: Error | null, bitmap: RoaringBitmap32 | undefined) => void

----------

### RoaringBitmap32ArrayCallback

```typescript
type RoaringBitmap32ArrayCallback = (error: Error | null, bitmap: RoaringBitmap32[] | undefined) => void;
```

**Type**

(error: Error | null, bitmap: RoaringBitmap32[] | undefined) => void

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
[InterfaceDeclaration-1]: index.d.md#roaringbitmap32statistics
[TypeAliasDeclaration-0]: index.d.md#roaringbitmap32callback
[TypeAliasDeclaration-1]: index.d.md#roaringbitmap32arraycallback
[ClassDeclaration-0]: index.d/roaringbitmap32.md#roaringbitmap32
[ClassDeclaration-1]: index.d/roaringbitmap32iterator.md#roaringbitmap32iterator
[VariableDeclaration-0]: index.d.md#instructionset
[VariableDeclaration-1]: index.d.md#sse42
[VariableDeclaration-2]: index.d.md#avx2
[VariableDeclaration-3]: index.d.md#croaringversion
[VariableDeclaration-4]: index.d.md#packageversion