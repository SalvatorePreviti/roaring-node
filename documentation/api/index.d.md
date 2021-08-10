# Table of contents

- [index.d.ts][sourcefile-0]
  - Interfaces
    - [RoaringBitmap32Statistics][interfacedeclaration-1]
  - Types
    - [RoaringBitmap32Callback][typealiasdeclaration-0]
    - [RoaringBitmap32ArrayCallback][typealiasdeclaration-1]
  - Variables
    - [instructionSet][variabledeclaration-0]
    - [CRoaringVersion][variabledeclaration-3]
    - [PackageVersion][variabledeclaration-4]

# index.d.ts

## Interfaces

### RoaringBitmap32Statistics

Object returned by RoaringBitmap32 statistics() method

```typescript
interface RoaringBitmap32Statistics {
  containers: number
  arrayContainers: number
  runContainers: number
  bitsetContainers: number
  valuesInArrayContainers: number
  valuesInRunContainers: number
  valuesInBitsetContainers: number
  bytesInArrayContainers: number
  bytesInRunContainers: number
  bytesInBitsetContainers: number
  maxValue: number
  minValue: number
  sumOfAllValues: number
  size: number
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
| valuesInBitsetContainers | number | false    | Number of values in bitmap containers.          |
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
type RoaringBitmap32Callback = (error: Error | null, bitmap: RoaringBitmap32 | undefined) => void
```

**Type**

(error: Error | null, bitmap: RoaringBitmap32 | undefined) => void

---

### RoaringBitmap32ArrayCallback

```typescript
type RoaringBitmap32ArrayCallback = (error: Error | null, bitmap: RoaringBitmap32[] | undefined) => void
```

**Type**

(error: Error | null, bitmap: RoaringBitmap32[] | undefined) => void

## Classes

### [RoaringBitmap32][classdeclaration-0]

Roaring bitmap that supports 32 bit unsigned integers.

- See http://roaringbitmap.org/
- See https://github.com/SalvatorePreviti/roaring-node

---

### [RoaringBitmap32Iterator][classdeclaration-1]

Iterator for RoaringBitmap32.

WARNING: Is not allowed to change the bitmap while iterating.
The iterator may throw exception if the bitmap is changed during the iteration.

## Variables

---

### CRoaringVersion

Property: The version of the CRoaring libary as a string.
Example: "0.2.42"

```typescript
var CRoaringVersion: string
```

**Type**

string

---

### PackageVersion

Property: The version of the roaring npm package as a string.
Example: "0.2.2"

```typescript
var PackageVersion: string
```

**Type**

string

[sourcefile-0]: index.d.md#indexdts
[interfacedeclaration-1]: index.d.md#roaringbitmap32statistics
[typealiasdeclaration-0]: index.d.md#roaringbitmap32callback
[typealiasdeclaration-1]: index.d.md#roaringbitmap32arraycallback
[classdeclaration-0]: index.d/roaringbitmap32.md#roaringbitmap32
[classdeclaration-1]: index.d/roaringbitmap32iterator.md#roaringbitmap32iterator
[variabledeclaration-0]: index.d.md#instructionset
[variabledeclaration-1]: index.d.md#sse42
[variabledeclaration-2]: index.d.md#avx2
[variabledeclaration-3]: index.d.md#croaringversion
[variabledeclaration-4]: index.d.md#packageversion
