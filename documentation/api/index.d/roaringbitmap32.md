# Table of contents

* [RoaringBitmap32][ClassDeclaration-0]
    * Constructor
        * [constructor(values)][Constructor-0]
    * Methods
        * [from(values)][MethodDeclaration-0]
        * [deserialize(serialized, portable)][MethodDeclaration-1]
        * [deserialize(serialized, portable)][MethodDeclaration-51]
        * [swap(a, b)][MethodDeclaration-2]
        * [and(a, b)][MethodDeclaration-3]
        * [or(a, b)][MethodDeclaration-4]
        * [xor(a, b)][MethodDeclaration-5]
        * [andNot(a, b)][MethodDeclaration-6]
        * [orMany(values)][MethodDeclaration-7]
        * [orMany(values)][MethodDeclaration-8]
        * [__@iterator()][MethodDeclaration-9]
        * [iterator()][MethodDeclaration-12]
        * [minimum()][MethodDeclaration-13]
        * [maximum()][MethodDeclaration-14]
        * [has(value)][MethodDeclaration-15]
        * [hasRange(rangeStart, rangeEnd)][MethodDeclaration-16]
        * [copyFrom(values)][MethodDeclaration-17]
        * [add(value)][MethodDeclaration-18]
        * [tryAdd(value)][MethodDeclaration-19]
        * [addMany(values)][MethodDeclaration-20]
        * [delete(value)][MethodDeclaration-21]
        * [remove(value)][MethodDeclaration-22]
        * [removeMany(values)][MethodDeclaration-23]
        * [flipRange(rangeStart, rangeEnd)][MethodDeclaration-24]
        * [addRange(rangeStart, rangeEnd)][MethodDeclaration-25]
        * [clear()][MethodDeclaration-26]
        * [orInPlace(values)][MethodDeclaration-27]
        * [andNotInPlace(values)][MethodDeclaration-28]
        * [andInPlace(values)][MethodDeclaration-29]
        * [xorInPlace(values)][MethodDeclaration-30]
        * [isSubset(other)][MethodDeclaration-31]
        * [isStrictSubset(other)][MethodDeclaration-32]
        * [isEqual(other)][MethodDeclaration-33]
        * [intersects(other)][MethodDeclaration-34]
        * [andCardinality(other)][MethodDeclaration-35]
        * [orCardinality(other)][MethodDeclaration-36]
        * [andNotCardinality(other)][MethodDeclaration-37]
        * [xorCardinality(other)][MethodDeclaration-38]
        * [jaccardIndex(other)][MethodDeclaration-39]
        * [removeRunCompression()][MethodDeclaration-40]
        * [runOptimize()][MethodDeclaration-41]
        * [shrinkToFit()][MethodDeclaration-42]
        * [rank(maxValue)][MethodDeclaration-43]
        * [select(rank)][MethodDeclaration-44]
        * [toUint32Array()][MethodDeclaration-45]
        * [toArray()][MethodDeclaration-46]
        * [toSet()][MethodDeclaration-47]
        * [toJSON()][MethodDeclaration-48]
        * [getSerializationSizeInBytes(portable)][MethodDeclaration-49]
        * [serialize(portable)][MethodDeclaration-50]
        * [clone()][MethodDeclaration-52]
        * [toString()][MethodDeclaration-53]
        * [contentToString(maxLength)][MethodDeclaration-54]
        * [statistics()][MethodDeclaration-55]
    * Properties
        * [size][PropertyDeclaration-0]
        * [isEmpty][PropertyDeclaration-1]

# RoaringBitmap32

Roaring bitmap that supports 32 bit unsigned integers.

- See http://roaringbitmap.org/
- See https://github.com/SalvatorePreviti/roaring-node

```typescript
class RoaringBitmap32 implements Iterable<number>
```
## Constructor

### constructor(values)

Creates an instance of RoaringBitmap32.
Is faster to pass a Uint32Array instance instead of an array or an iterable.
Is even faster if the given argument is a RoaringBitmap32 (performs a fast copy).

```typescript
public constructor(values?: Iterable<number> | undefined<number>);
```

**Parameters**

| Name   | Type                                      |
| ------ | ----------------------------------------- |
| values | Iterable<number> &#124; undefined<number> |

## Methods

### from(values)

Creates an instance of RoaringBitmap32 from the given Iterable.
Is faster to pass a Uint32Array instance instead of an array or an iterable.
Is optimized if the given argument is a RoaringBitmap32 (performs a fast copy).

```typescript
public static from(values: Iterable<number>): RoaringBitmap32;
```

**Parameters**

| Name   | Type             | Description        |
| ------ | ---------------- | ------------------ |
| values | Iterable<number> | The values to set. |

**Return type**

[RoaringBitmap32][ClassDeclaration-0]

----------

### deserialize(serialized, portable)

Deserializes the bitmap from an Uint8Array or a Buffer.
Returns a new RoaringBitmap32 instance.

Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
The portable version is meant to be compatible with Java and Go versions.

```typescript
public static deserialize(serialized: Uint8Array, portable?: boolean | undefined): RoaringBitmap32;
```

**Parameters**

| Name       | Type                     | Description                                                                                        |
| ---------- | ------------------------ | -------------------------------------------------------------------------------------------------- |
| serialized | Uint8Array               | An Uint8Array or a node Buffer that contains the serialized data.                                  |
| portable   | boolean &#124; undefined | If false (default), optimized C/C++ format is used.  If true, Java and Go portable format is used. |

**Return type**

[RoaringBitmap32][ClassDeclaration-0]

----------

### deserialize(serialized, portable)

Deserializes the bitmap from an Uint8Array or a Buffer.

Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
The portable version is meant to be compatible with Java and Go versions.

```typescript
public deserialize(serialized: Uint8Array, portable?: boolean | undefined): void;
```

**Parameters**

| Name       | Type                     | Description                                                                                       |
| ---------- | ------------------------ | ------------------------------------------------------------------------------------------------- |
| serialized | Uint8Array               | An Uint8Array or a node Buffer that contains the serialized data.                                 |
| portable   | boolean &#124; undefined | If false (default), optimized C/C++ format is used. If true, Java and Go portable format is used. |

**Return type**

void

----------

### swap(a, b)

Swaps the content of two RoaringBitmap32 instances.

```typescript
public static swap(a: RoaringBitmap32, b: RoaringBitmap32): void;
```

**Parameters**

| Name | Type                                  | Description                             |
| ---- | ------------------------------------- | --------------------------------------- |
| a    | [RoaringBitmap32][ClassDeclaration-0] | First RoaringBitmap32 instance to swap  |
| b    | [RoaringBitmap32][ClassDeclaration-0] | Second RoaringBitmap32 instance to swap |

**Return type**

void

----------

### and(a, b)

Returns a new RoaringBitmap32 with the intersection (and) between the given two bitmaps.
The provided bitmaps are not modified.

```typescript
public static and(a: RoaringBitmap32, b: RoaringBitmap32): RoaringBitmap32;
```

**Parameters**

| Name | Type                                  | Description                                 |
| ---- | ------------------------------------- | ------------------------------------------- |
| a    | [RoaringBitmap32][ClassDeclaration-0] | The first RoaringBitmap32 instance to and.  |
| b    | [RoaringBitmap32][ClassDeclaration-0] | The second RoaringBitmap32 instance to and. |

**Return type**

[RoaringBitmap32][ClassDeclaration-0]

----------

### or(a, b)

Returns a new RoaringBitmap32 with the union (or) of the two given bitmaps.
The provided bitmaps are not modified.

```typescript
public static or(a: RoaringBitmap32, b: RoaringBitmap32): RoaringBitmap32;
```

**Parameters**

| Name | Type                                  | Description                                |
| ---- | ------------------------------------- | ------------------------------------------ |
| a    | [RoaringBitmap32][ClassDeclaration-0] | The first RoaringBitmap32 instance to or.  |
| b    | [RoaringBitmap32][ClassDeclaration-0] | The second RoaringBitmap32 instance to or. |

**Return type**

[RoaringBitmap32][ClassDeclaration-0]

----------

### xor(a, b)

Returns a new RoaringBitmap32 with the symmetric union (xor) between the two given bitmaps.
The provided bitmaps are not modified.

```typescript
public static xor(a: RoaringBitmap32, b: RoaringBitmap32): RoaringBitmap32;
```

**Parameters**

| Name | Type                                  | Description                                 |
| ---- | ------------------------------------- | ------------------------------------------- |
| a    | [RoaringBitmap32][ClassDeclaration-0] | The first RoaringBitmap32 instance to xor.  |
| b    | [RoaringBitmap32][ClassDeclaration-0] | The second RoaringBitmap32 instance to xor. |

**Return type**

[RoaringBitmap32][ClassDeclaration-0]

----------

### andNot(a, b)

Returns a new RoaringBitmap32 with the difference (and not) between the two given bitmaps.
The provided bitmaps are not modified.

```typescript
public static andNot(a: RoaringBitmap32, b: RoaringBitmap32): RoaringBitmap32;
```

**Parameters**

| Name | Type                                  | Description                          |
| ---- | ------------------------------------- | ------------------------------------ |
| a    | [RoaringBitmap32][ClassDeclaration-0] | The first RoaringBitmap32 instance.  |
| b    | [RoaringBitmap32][ClassDeclaration-0] | The second RoaringBitmap32 instance. |

**Return type**

[RoaringBitmap32][ClassDeclaration-0]

----------

### orMany(values)

Performs a union between all the given array of RoaringBitmap32 instances.
This function is faster than calling or multiple times.
Performs a union between all the given RoaringBitmap32 instances.
This function is faster than calling or multiple times.

```typescript
public static orMany(values: RoaringBitmap32[]): RoaringBitmap32;
```

**Parameters**

| Name   | Type                                    | Description                                           |
| ------ | --------------------------------------- | ----------------------------------------------------- |
| values | [RoaringBitmap32][ClassDeclaration-0][] | An array of RoaringBitmap32 instances to or together. |

**Return type**

[RoaringBitmap32][ClassDeclaration-0]

----------

### orMany(values)

Performs a union between all the given array of RoaringBitmap32 instances.
This function is faster than calling or multiple times.
Performs a union between all the given RoaringBitmap32 instances.
This function is faster than calling or multiple times.

```typescript
public static orMany(values: RoaringBitmap32[]): RoaringBitmap32;
```

**Parameters**

| Name   | Type                                    | Description                                   |
| ------ | --------------------------------------- | --------------------------------------------- |
| values | [RoaringBitmap32][ClassDeclaration-0][] | The RoaringBitmap32 instances to or together. |

**Return type**

[RoaringBitmap32][ClassDeclaration-0]

----------

### __@iterator()

[Symbol.iterator]() Gets a new iterator able to iterate all values in the set in order.

```typescript
public __@iterator(): RoaringBitmap32Iterator;
```

**Return type**

[RoaringBitmap32Iterator][ClassDeclaration-1]

----------

### iterator()

Gets a new iterator able to iterate all values in the set in order.
Same as [Symbol.iterator]()

```typescript
public iterator(): RoaringBitmap32Iterator;
```

**Return type**

[RoaringBitmap32Iterator][ClassDeclaration-1]

----------

### minimum()

Gets the minimum value in the set.

```typescript
public minimum(): number;
```

**Return type**

number

----------

### maximum()

Gets the maximum value in the set.

```typescript
public maximum(): number;
```

**Return type**

number

----------

### has(value)

Checks wether the given value exists in the set.

```typescript
public has(value: number): boolean;
```

**Parameters**

| Name  | Type   | Description                          |
| ----- | ------ | ------------------------------------ |
| value | number | A 32 bit unsigned integer to search. |

**Return type**

boolean

----------

### hasRange(rangeStart, rangeEnd)

Check whether a range of values from range_start (included) to range_end (excluded) is present

```typescript
public hasRange(rangeStart: number, rangeEnd: number): boolean;
```

**Parameters**

| Name       | Type   | Description      |
| ---------- | ------ | ---------------- |
| rangeStart | number | The start index. |
| rangeEnd   | number | The end index.   |

**Return type**

boolean

----------

### copyFrom(values)

Overwrite the content of this bitmap copying it from an Iterable or another RoaringBitmap32.
Is faster to pass a Uint32Array instance instead of an array or an iterable.
Is even faster if a RoaringBitmap32 instance is used (it performs a simple copy).

```typescript
public copyFrom(values: Iterable<number>): void;
```

**Parameters**

| Name   | Type             | Description                                   |
| ------ | ---------------- | --------------------------------------------- |
| values | Iterable<number> | The new values or a RoaringBitmap32 instance. |

**Return type**

void

----------

### add(value)

Adds a single value to the set.

```typescript
public add(value: number): RoaringBitmap32;
```

**Parameters**

| Name  | Type   | Description                                                |
| ----- | ------ | ---------------------------------------------------------- |
| value | number | The value to add. Must be a valid 32 bit unsigned integer. |

**Return type**

[RoaringBitmap32][ClassDeclaration-0]

----------

### tryAdd(value)

Tries to add a single value to the set.
Returns true if the value was added during this call, false if already existing or not a valid unsigned integer 32.

```typescript
public tryAdd(value: number): boolean;
```

**Parameters**

| Name  | Type   | Description                                                |
| ----- | ------ | ---------------------------------------------------------- |
| value | number | The value to add. Must be a valid 32 bit unsigned integer. |

**Return type**

boolean

----------

### addMany(values)

Adds multiple values to the set.
Faster than calling add() multiple times.
It is faster to insert sorted or partially sorted values.
Is faster to use Uint32Array instead of arrays or iterables.
Is optimized if the argument is an instance of RoaringBitmap32 (it performs an OR union).

```typescript
public addMany(values: Iterable<number>): RoaringBitmap32;
```

**Parameters**

| Name   | Type             | Description                      |
| ------ | ---------------- | -------------------------------- |
| values | Iterable<number> | An iterable of values to insert. |

**Return type**

[RoaringBitmap32][ClassDeclaration-0]

----------

### delete(value)

Removes a value from the set.
Returns true if the value was removed during this call, false if not.

```typescript
public delete(value: number): boolean;
```

**Parameters**

| Name  | Type   | Description                            |
| ----- | ------ | -------------------------------------- |
| value | number | The unsigned 32 bit integer to remove. |

**Return type**

boolean

----------

### remove(value)

Removes a value from the set.

```typescript
public remove(value: number): void;
```

**Parameters**

| Name  | Type   | Description                            |
| ----- | ------ | -------------------------------------- |
| value | number | The unsigned 32 bit integer to remove. |

**Return type**

void

----------

### removeMany(values)

Removes multiple values from the set.
Faster than calling remove() multiple times.
Is faster to use Uint32Array instead of arrays or iterables.
This function is optimized if the argument is an instance of RoaringBitmap32 (it performs an AND NOT operation).

```typescript
public removeMany(values: Iterable<number>): RoaringBitmap32;
```

**Parameters**

| Name   | Type             | Description                      |
| ------ | ---------------- | -------------------------------- |
| values | Iterable<number> | An iterable of values to remove. |

**Return type**

[RoaringBitmap32][ClassDeclaration-0]

----------

### flipRange(rangeStart, rangeEnd)

Negates (in place) the roaring bitmap within a specified interval: [rangeStart, rangeEnd).
First element is included, last element is excluded.
The number of negated values is rangeEnd - rangeStart.
Areas outside the range are passed through unchanged.

```typescript
public flipRange(rangeStart: number, rangeEnd: number): void;
```

**Parameters**

| Name       | Type   | Description                           |
| ---------- | ------ | ------------------------------------- |
| rangeStart | number | The start index. Trimmed to 0.        |
| rangeEnd   | number | The end index. Trimmed to 4294967297. |

**Return type**

void

----------

### addRange(rangeStart, rangeEnd)

Adds all the values in the interval: [rangeStart, rangeEnd).
First element is included, last element is excluded.
The number of added values is rangeEnd - rangeStart.
Areas outside the range are passed through unchanged.

```typescript
public addRange(rangeStart: number, rangeEnd: number): void;
```

**Parameters**

| Name       | Type   | Description                           |
| ---------- | ------ | ------------------------------------- |
| rangeStart | number | The start index. Trimmed to 0.        |
| rangeEnd   | number | The end index. Trimmed to 4294967297. |

**Return type**

void

----------

### clear()

Removes all values from the set.
It frees resources, if needed you can use clear to free some memory before the garbage collector disposes this instance.

```typescript
public clear(): void;
```

**Return type**

void

----------

### orInPlace(values)

Performs an union in place ("this = this OR values").
Is faster to use Uint32Array instead of arrays or iterables.
This function is optimized if the argument is an instance of RoaringBitmap32.
Same as addMany.

```typescript
public orInPlace(values: Iterable<number>): RoaringBitmap32;
```

**Parameters**

| Name   | Type             | Description                                                            |
| ------ | ---------------- | ---------------------------------------------------------------------- |
| values | Iterable<number> | A RoaringBitmap32 instance or an iterable of unsigned 32 bit integers. |

**Return type**

[RoaringBitmap32][ClassDeclaration-0]

----------

### andNotInPlace(values)

Performs a AND NOT operation in place ("this = this AND NOT values").
Is faster to use Uint32Array instead of arrays or iterables.
This function is optimized if the argument is an instance of RoaringBitmap32.
Same as removeMany.

```typescript
public andNotInPlace(values: Iterable<number>): RoaringBitmap32;
```

**Parameters**

| Name   | Type             | Description                                                            |
| ------ | ---------------- | ---------------------------------------------------------------------- |
| values | Iterable<number> | A RoaringBitmap32 instance or an iterable of unsigned 32 bit integers. |

**Return type**

[RoaringBitmap32][ClassDeclaration-0]

----------

### andInPlace(values)

Performs the intersection (and) between the current bitmap and the provided bitmap,
writing the result in the current bitmap.
Is faster to use Uint32Array instead of arrays or iterables.
This function is optimized if the argument is an instance of RoaringBitmap32.
The provided bitmap is not modified.

```typescript
public andInPlace(values: Iterable<number>): RoaringBitmap32;
```

**Parameters**

| Name   | Type             | Description                                                            |
| ------ | ---------------- | ---------------------------------------------------------------------- |
| values | Iterable<number> | A RoaringBitmap32 instance or an iterable of unsigned 32 bit integers. |

**Return type**

[RoaringBitmap32][ClassDeclaration-0]

----------

### xorInPlace(values)

Performs the symmetric union (xor) between the current bitmap and the provided bitmap,
writing the result in the current bitmap.
Is faster to use Uint32Array instead of arrays or iterables.
This function is optimized if the argument is an instance of RoaringBitmap32.
The provided bitmap is not modified.

```typescript
public xorInPlace(values: Iterable<number>): RoaringBitmap32;
```

**Parameters**

| Name   | Type             | Description                                                            |
| ------ | ---------------- | ---------------------------------------------------------------------- |
| values | Iterable<number> | A RoaringBitmap32 instance or an iterable of unsigned 32 bit integers. |

**Return type**

[RoaringBitmap32][ClassDeclaration-0]

----------

### isSubset(other)

Checks wether this set is a subset or the same as the given set.
Returns false also if the given argument is not a RoaringBitmap32 instance.

```typescript
public isSubset(other: RoaringBitmap32): boolean;
```

**Parameters**

| Name  | Type                                  | Description    |
| ----- | ------------------------------------- | -------------- |
| other | [RoaringBitmap32][ClassDeclaration-0] | The other set. |

**Return type**

boolean

----------

### isStrictSubset(other)

Checks wether this set is a strict subset of the given set.
Returns false if the sets are the same.
Returns false also if the given argument is not a RoaringBitmap32 instance.

```typescript
public isStrictSubset(other: RoaringBitmap32): boolean;
```

**Parameters**

| Name  | Type                                  | Description                         |
| ----- | ------------------------------------- | ----------------------------------- |
| other | [RoaringBitmap32][ClassDeclaration-0] | The other RoaringBitmap32 instance. |

**Return type**

boolean

----------

### isEqual(other)

Checks wether this set is equal to another set.
Returns false also if the given argument is not a RoaringBitmap32 instance.

```typescript
public isEqual(other: RoaringBitmap32): boolean;
```

**Parameters**

| Name  | Type                                  | Description                            |
| ----- | ------------------------------------- | -------------------------------------- |
| other | [RoaringBitmap32][ClassDeclaration-0] | The other set to compare for equality. |

**Return type**

boolean

----------

### intersects(other)

Check whether the two bitmaps intersect.
Returns true if there is at least one item in common, false if not.
Returns false also if the given argument is not a RoaringBitmap32 instance.

```typescript
public intersects(other: RoaringBitmap32): boolean;
```

**Parameters**

| Name  | Type                                  | Description                                |
| ----- | ------------------------------------- | ------------------------------------------ |
| other | [RoaringBitmap32][ClassDeclaration-0] | The other set to compare for intersection. |

**Return type**

boolean

----------

### andCardinality(other)

Computes the size of the intersection between two bitmaps (the number of values in common).
Returns -1 if the given argument is not a RoaringBitmap32 instance.

```typescript
public andCardinality(other: RoaringBitmap32): number;
```

**Parameters**

| Name  | Type                                  | Description                                |
| ----- | ------------------------------------- | ------------------------------------------ |
| other | [RoaringBitmap32][ClassDeclaration-0] | The other set to compare for intersection. |

**Return type**

number

----------

### orCardinality(other)

Computes the size of the union between two bitmaps.
Returns -1 if the given argument is not a RoaringBitmap32 instance.

```typescript
public orCardinality(other: RoaringBitmap32): number;
```

**Parameters**

| Name  | Type                                  | Description                                |
| ----- | ------------------------------------- | ------------------------------------------ |
| other | [RoaringBitmap32][ClassDeclaration-0] | The other set to compare for intersection. |

**Return type**

number

----------

### andNotCardinality(other)

Computes the size of the difference (andnot) between two bitmaps.
Returns -1 if the given argument is not a RoaringBitmap32 instance.

```typescript
public andNotCardinality(other: RoaringBitmap32): number;
```

**Parameters**

| Name  | Type                                  | Description                                |
| ----- | ------------------------------------- | ------------------------------------------ |
| other | [RoaringBitmap32][ClassDeclaration-0] | The other set to compare for intersection. |

**Return type**

number

----------

### xorCardinality(other)

Computes the size of the symmetric difference (xor) between two bitmaps.
Returns -1 if the given argument is not a RoaringBitmap32 instance.

```typescript
public xorCardinality(other: RoaringBitmap32): number;
```

**Parameters**

| Name  | Type                                  | Description                                |
| ----- | ------------------------------------- | ------------------------------------------ |
| other | [RoaringBitmap32][ClassDeclaration-0] | The other set to compare for intersection. |

**Return type**

number

----------

### jaccardIndex(other)

Computes the Jaccard index between two bitmaps.
(Also known as the Tanimoto distance or the Jaccard similarity coefficient).
See https://en.wikipedia.org/wiki/Jaccard_index

The Jaccard index is undefined if both bitmaps are empty.

Returns -1 if the given argument is not a RoaringBitmap32 instance.

```typescript
public jaccardIndex(other: RoaringBitmap32): number;
```

**Parameters**

| Name  | Type                                  | Description                           |
| ----- | ------------------------------------- | ------------------------------------- |
| other | [RoaringBitmap32][ClassDeclaration-0] | The other RoaringBitmap32 to compare. |

**Return type**

number

----------

### removeRunCompression()

Remove run-length encoding even when it is more space efficient.
Return whether a change was applied.

```typescript
public removeRunCompression(): boolean;
```

**Return type**

boolean

----------

### runOptimize()

Convert array and bitmap containers to run containers when it is more efficient;
also convert from run containers when more space efficient.
Returns true if the bitmap has at least one run container.
Additional savings might be possible by calling shrinkToFit().

```typescript
public runOptimize(): boolean;
```

**Return type**

boolean

----------

### shrinkToFit()

If needed, reallocate memory to shrink the memory usage.
Returns the number of bytes saved.

```typescript
public shrinkToFit(): number;
```

**Return type**

number

----------

### rank(maxValue)

Returns the number of values in the set that are smaller or equal to the given value.

```typescript
public rank(maxValue: number): number;
```

**Parameters**

| Name     | Type   | Description       |
| -------- | ------ | ----------------- |
| maxValue | number | The maximum value |

**Return type**

number

----------

### select(rank)

If the size of the roaring bitmap is strictly greater than rank,
then this function returns the element of given rank.
Otherwise, it returns undefined.

```typescript
public select(rank: number): number | undefined;
```

**Parameters**

| Name | Type   | Description                           |
| ---- | ------ | ------------------------------------- |
| rank | number | The rank, an unsigned 32 bit integer. |

**Return type**

number | undefined

----------

### toUint32Array()

Creates a new Uint32Array and fills it with all the values in the bitmap.
The returned array may be very big, up to 4 gigabytes.
Use this function only when you know what you are doing.
This function is faster than calling new Uint32Array(bitmap);

```typescript
public toUint32Array(): Uint32Array;
```

**Return type**

Uint32Array

----------

### toArray()

Creates a new plain JS array and fills it with all the values in the bitmap.
The returned array may be very big, use this function only when you know what you are doing.

```typescript
public toArray(): number[];
```

**Return type**

number[]

----------

### toSet()

Creates a new plain JS Set<number> and fills it with all the values in the bitmap.
The returned set may be very big, use this function only when you know what you are doing.

```typescript
public toSet(): Set<number>;
```

**Return type**

Set<number>

----------

### toJSON()

Returns a plain JS array with all the values in the bitmap.
Used by JSON.stringify to serialize this bitmap as an array.

```typescript
public toJSON(): number[];
```

**Return type**

number[]

----------

### getSerializationSizeInBytes(portable)

How many bytes are required to serialize this bitmap.

Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
The portable version is meant to be compatible with Java and Go versions.

```typescript
public getSerializationSizeInBytes(portable?: boolean | undefined): number;
```

**Parameters**

| Name     | Type                     | Description                                                                                       |
| -------- | ------------------------ | ------------------------------------------------------------------------------------------------- |
| portable | boolean &#124; undefined | If false (default), optimized C/C++ format is used. If true, Java and Go portable format is used. |

**Return type**

number

----------

### serialize(portable)

Serializes the bitmap into a new Buffer.

Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
The portable version is meant to be compatible with Java and Go versions.

```typescript
public serialize(portable?: boolean | undefined): Buffer;
```

**Parameters**

| Name     | Type                     | Description                                                                                       |
| -------- | ------------------------ | ------------------------------------------------------------------------------------------------- |
| portable | boolean &#124; undefined | If false (default), optimized C/C++ format is used. If true, Java and Go portable format is used. |

**Return type**

[Buffer][InterfaceDeclaration-0]

----------

### clone()

Returns a new RoaringBitmap32 that is a copy of this bitmap.
Same as new RoaringBitmap32(copy)

```typescript
public clone(): RoaringBitmap32;
```

**Return type**

[RoaringBitmap32][ClassDeclaration-0]

----------

### toString()

Returns "RoaringBitmap32(size)".
To have a standard string representation of the content as a string, call contentToString() instead.

```typescript
public toString(): string;
```

**Return type**

string

----------

### contentToString(maxLength)

Returns a standard string representation of the content of this RoaringBitmap32 instance. It may return a very long string.
Default max length is 32000 characters, everything after maxLength is truncated (ellipsis added).

```typescript
public contentToString(maxLength?: number | undefined): string;
```

**Parameters**

| Name      | Type                    | Description                                                                                                 |
| --------- | ----------------------- | ----------------------------------------------------------------------------------------------------------- |
| maxLength | number &#124; undefined | Approximate maximum length of the string. Default is 32000. Ellipsis will be added if the string is longer. |

**Return type**

string

----------

### statistics()

Returns an object that contains statistic information about this RoaringBitmap32 instance.

```typescript
public statistics(): { containers: number; arrayContainers: number; runContainers: number; bitsetContainers: number; valuesInArrayContainers: number; valuesInRunContainers: number; valuesInBitsetContainers: number; bytesInArrayContainers: number; bytesInRunContainers: number; bytesInBitsetContainers: number; maxValue: number; minValue: number; sumOfAllValues: number; size: number; };
```

**Return type**

{ containers: number; arrayContainers: number; runContainers: number; bitsetContainers: number; valuesInArrayContainers: number; valuesInRunContainers: number; valuesInBitsetContainers: number; bytesInArrayContainers: number; bytesInRunContainers: number; bytesInBitsetContainers: number; maxValue: number; minValue: number; sumOfAllValues: number; size: number; }

## Properties

### size

Property. Gets the number of items in the set (cardinality).

```typescript
public readonly size: number;
```

**Type**

number

----------

### isEmpty

Property. True if the bitmap is empty.

```typescript
public readonly isEmpty: boolean;
```

**Type**

boolean

[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[Constructor-0]: roaringbitmap32.md#constructorvalues
[MethodDeclaration-0]: roaringbitmap32.md#fromvalues
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[MethodDeclaration-1]: roaringbitmap32.md#deserializeserialized-portable
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[MethodDeclaration-51]: roaringbitmap32.md#deserializeserialized-portable
[MethodDeclaration-2]: roaringbitmap32.md#swapa-b
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[MethodDeclaration-3]: roaringbitmap32.md#anda-b
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[MethodDeclaration-4]: roaringbitmap32.md#ora-b
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[MethodDeclaration-5]: roaringbitmap32.md#xora-b
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[MethodDeclaration-6]: roaringbitmap32.md#andnota-b
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[MethodDeclaration-7]: roaringbitmap32.md#ormanyvalues
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[MethodDeclaration-8]: roaringbitmap32.md#ormanyvalues
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[MethodDeclaration-9]: roaringbitmap32.md#__iterator
[ClassDeclaration-1]: roaringbitmap32iterator.md#roaringbitmap32iterator
[MethodDeclaration-12]: roaringbitmap32.md#iterator
[ClassDeclaration-1]: roaringbitmap32iterator.md#roaringbitmap32iterator
[MethodDeclaration-13]: roaringbitmap32.md#minimum
[MethodDeclaration-14]: roaringbitmap32.md#maximum
[MethodDeclaration-15]: roaringbitmap32.md#hasvalue
[MethodDeclaration-16]: roaringbitmap32.md#hasrangerangestart-rangeend
[MethodDeclaration-17]: roaringbitmap32.md#copyfromvalues
[MethodDeclaration-18]: roaringbitmap32.md#addvalue
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[MethodDeclaration-19]: roaringbitmap32.md#tryaddvalue
[MethodDeclaration-20]: roaringbitmap32.md#addmanyvalues
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[MethodDeclaration-21]: roaringbitmap32.md#deletevalue
[MethodDeclaration-22]: roaringbitmap32.md#removevalue
[MethodDeclaration-23]: roaringbitmap32.md#removemanyvalues
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[MethodDeclaration-24]: roaringbitmap32.md#fliprangerangestart-rangeend
[MethodDeclaration-25]: roaringbitmap32.md#addrangerangestart-rangeend
[MethodDeclaration-26]: roaringbitmap32.md#clear
[MethodDeclaration-27]: roaringbitmap32.md#orinplacevalues
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[MethodDeclaration-28]: roaringbitmap32.md#andnotinplacevalues
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[MethodDeclaration-29]: roaringbitmap32.md#andinplacevalues
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[MethodDeclaration-30]: roaringbitmap32.md#xorinplacevalues
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[MethodDeclaration-31]: roaringbitmap32.md#issubsetother
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[MethodDeclaration-32]: roaringbitmap32.md#isstrictsubsetother
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[MethodDeclaration-33]: roaringbitmap32.md#isequalother
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[MethodDeclaration-34]: roaringbitmap32.md#intersectsother
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[MethodDeclaration-35]: roaringbitmap32.md#andcardinalityother
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[MethodDeclaration-36]: roaringbitmap32.md#orcardinalityother
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[MethodDeclaration-37]: roaringbitmap32.md#andnotcardinalityother
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[MethodDeclaration-38]: roaringbitmap32.md#xorcardinalityother
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[MethodDeclaration-39]: roaringbitmap32.md#jaccardindexother
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[MethodDeclaration-40]: roaringbitmap32.md#removeruncompression
[MethodDeclaration-41]: roaringbitmap32.md#runoptimize
[MethodDeclaration-42]: roaringbitmap32.md#shrinktofit
[MethodDeclaration-43]: roaringbitmap32.md#rankmaxvalue
[MethodDeclaration-44]: roaringbitmap32.md#selectrank
[MethodDeclaration-45]: roaringbitmap32.md#touint32array
[MethodDeclaration-46]: roaringbitmap32.md#toarray
[MethodDeclaration-47]: roaringbitmap32.md#toset
[MethodDeclaration-48]: roaringbitmap32.md#tojson
[MethodDeclaration-49]: roaringbitmap32.md#getserializationsizeinbytesportable
[MethodDeclaration-50]: roaringbitmap32.md#serializeportable
[InterfaceDeclaration-0]: ../index.d.md#indexdts
[MethodDeclaration-52]: roaringbitmap32.md#clone
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[MethodDeclaration-53]: roaringbitmap32.md#tostring
[MethodDeclaration-54]: roaringbitmap32.md#contenttostringmaxlength
[MethodDeclaration-55]: roaringbitmap32.md#statistics
[PropertyDeclaration-0]: roaringbitmap32.md#size
[PropertyDeclaration-1]: roaringbitmap32.md#isempty