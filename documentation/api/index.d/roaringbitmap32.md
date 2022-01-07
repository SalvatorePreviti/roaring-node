# Table of contents

- [RoaringBitmap32][classdeclaration-0]
  - Constructor
    - [constructor(values)][constructor-0]
  - Methods
    - [from(values)][methoddeclaration-0]
    - [fromRange(rangeStart, rangeEnd, step)][methoddeclaration-1]
    - [fromArrayAsync(values)][methoddeclaration-2]
    - [fromArrayAsync(values, callback)][methoddeclaration-3]
    - [deserialize(serialized, portable)][methoddeclaration-4]
    - [deserialize(serialized, portable)][methoddeclaration-67]
    - [deserializeAsync(serialized, portable)][methoddeclaration-5]
    - [deserializeAsync(serialized, portable, callback)][methoddeclaration-6]
    - [deserializeParallelAsync(serialized, portable)][methoddeclaration-7]
    - [deserializeParallelAsync(serialized, portable, callback)][methoddeclaration-8]
    - [swap(a, b)][methoddeclaration-9]
    - [and(a, b)][methoddeclaration-10]
    - [or(a, b)][methoddeclaration-11]
    - [xor(a, b)][methoddeclaration-12]
    - [andNot(a, b)][methoddeclaration-13]
    - [orMany(values)][methoddeclaration-14]
    - [orMany(values)][methoddeclaration-15]
    - [xorMany(values)][methoddeclaration-16]
    - [xorMany(values)][methoddeclaration-17]
    - [\_\_@iterator()][methoddeclaration-18]
    - [iterator()][methoddeclaration-21]
    - [keys()][methoddeclaration-22]
    - [values()][methoddeclaration-23]
    - [entries()][methoddeclaration-24]
    - [forEach(callbackfn, thisArg)][methoddeclaration-25]
    - [minimum()][methoddeclaration-26]
    - [maximum()][methoddeclaration-27]
    - [has(value)][methoddeclaration-28]
    - [hasRange(rangeStart, rangeEnd)][methoddeclaration-29]
    - [rangeCardinality(rangeStart, rangeEnd)][methoddeclaration-30]
    - [copyFrom(values)][methoddeclaration-31]
    - [add(value)][methoddeclaration-32]
    - [tryAdd(value)][methoddeclaration-33]
    - [addMany(values)][methoddeclaration-34]
    - [delete(value)][methoddeclaration-35]
    - [remove(value)][methoddeclaration-36]
    - [removeMany(values)][methoddeclaration-37]
    - [flipRange(rangeStart, rangeEnd)][methoddeclaration-38]
    - [addRange(rangeStart, rangeEnd)][methoddeclaration-39]
    - [removeRange(rangeStart, rangeEnd)][methoddeclaration-40]
    - [clear()][methoddeclaration-41]
    - [orInPlace(values)][methoddeclaration-42]
    - [andNotInPlace(values)][methoddeclaration-43]
    - [andInPlace(values)][methoddeclaration-44]
    - [xorInPlace(values)][methoddeclaration-45]
    - [isSubset(other)][methoddeclaration-46]
    - [isStrictSubset(other)][methoddeclaration-47]
    - [isEqual(other)][methoddeclaration-48]
    - [intersects(other)][methoddeclaration-49]
    - [andCardinality(other)][methoddeclaration-50]
    - [orCardinality(other)][methoddeclaration-51]
    - [andNotCardinality(other)][methoddeclaration-52]
    - [xorCardinality(other)][methoddeclaration-53]
    - [jaccardIndex(other)][methoddeclaration-54]
    - [removeRunCompression()][methoddeclaration-55]
    - [runOptimize()][methoddeclaration-56]
    - [shrinkToFit()][methoddeclaration-57]
    - [rank(maxValue)][methoddeclaration-58]
    - [select(rank)][methoddeclaration-59]
    - [toUint32Array()][methoddeclaration-60]
    - [rangeUint32Array(offset, limit)][methoddeclaration-61]
    - [toArray()][methoddeclaration-62]
    - [toSet()][methoddeclaration-63]
    - [toJSON()][methoddeclaration-64]
    - [getSerializationSizeInBytes(portable)][methoddeclaration-65]
    - [serialize(portable)][methoddeclaration-66]
    - [clone()][methoddeclaration-68]
    - [toString()][methoddeclaration-69]
    - [contentToString(maxLength)][methoddeclaration-70]
    - [statistics()][methoddeclaration-71]
  - Properties
    - [CRoaringVersion][propertydeclaration-0]
    - [CRoaringVersion][propertydeclaration-2]
    - [PackageVersion][propertydeclaration-1]
    - [PackageVersion][propertydeclaration-3]
    - [size][propertydeclaration-4]
    - [isEmpty][propertydeclaration-5]
    - [\_\_@toStringTag][propertydeclaration-6]

# RoaringBitmap32

Roaring bitmap that supports 32 bit unsigned integers.

- See http://roaringbitmap.org/
- See https://github.com/SalvatorePreviti/roaring-node

```typescript
class RoaringBitmap32 implements Set<number>
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

[RoaringBitmap32][classdeclaration-0]

---

### fromRange(rangeStart, rangeEnd, step)

Creates a new bitmap that contains all the values in the interval: [rangeStart, rangeEnd).
Is possible to specify the step parameter to have a non contiguous range.

```typescript
public static fromRange(rangeStart: number, rangeEnd: number, step?: number | undefined): RoaringBitmap32;
```

**Parameters**

| Name       | Type                    | Description                           |
| ---------- | ----------------------- | ------------------------------------- |
| rangeStart | number                  | The start index. Trimmed to 0.        |
| rangeEnd   | number                  | The end index. Trimmed to 4294967297. |
| step       | number &#124; undefined | The increment step, defaults to 1.    |

**Return type**

[RoaringBitmap32][classdeclaration-0]

---

### fromArrayAsync(values)

Creates an instance of RoaringBitmap32 from the given Iterable asynchronously in a parallel thread.

If a plain array or a plain iterable is passed, a temporary Uint32Array will be created synchronously.

NOTE: This method will throw a TypeError if a RoaringBitmap32 is passed as argument.

Returns a Promise that resolves to a new RoaringBitmap32 instance.
Creates an instance of RoaringBitmap32 from the given Iterable asynchronously in a parallel thread.

If a plain array or a plain iterable is passed, a temporary Uint32Array will be created synchronously.

NOTE: This method will throw a TypeError if a RoaringBitmap32 is passed as argument.

When deserialization is completed or failed, the given callback will be executed.

```typescript
public static fromArrayAsync(values: Iterable<number> | null | undefined): Promise<RoaringBitmap32>;
```

**Parameters**

| Name   | Type                                          | Description                                     |
| ------ | --------------------------------------------- | ----------------------------------------------- |
| values | Iterable<number> &#124; null &#124; undefined | The values to set. Cannot be a RoaringBitmap32. |

**Return type**

Promise<[RoaringBitmap32][classdeclaration-0]>

---

### fromArrayAsync(values, callback)

Creates an instance of RoaringBitmap32 from the given Iterable asynchronously in a parallel thread.

If a plain array or a plain iterable is passed, a temporary Uint32Array will be created synchronously.

NOTE: This method will throw a TypeError if a RoaringBitmap32 is passed as argument.

Returns a Promise that resolves to a new RoaringBitmap32 instance.
Creates an instance of RoaringBitmap32 from the given Iterable asynchronously in a parallel thread.

If a plain array or a plain iterable is passed, a temporary Uint32Array will be created synchronously.

NOTE: This method will throw a TypeError if a RoaringBitmap32 is passed as argument.

When deserialization is completed or failed, the given callback will be executed.

```typescript
public static fromArrayAsync(values: Iterable<number> | null | undefined, callback: RoaringBitmap32Callback): void;
```

**Parameters**

| Name     | Type                                              | Description                                           |
| -------- | ------------------------------------------------- | ----------------------------------------------------- |
| values   | Iterable<number> &#124; null &#124; undefined     | The values to set. Cannot be a RoaringBitmap32.       |
| callback | [RoaringBitmap32Callback][typealiasdeclaration-0] | The callback to execute when the operation completes. |

**Return type**

void

---

### deserialize(serialized, portable)

Deserializes the bitmap from an Uint8Array or a Buffer.

Returns a new RoaringBitmap32 instance.

Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
The portable version is meant to be compatible with Java and Go versions.

NOTE: this field was optional before, now is required and an Error is thrown if the portable flag is not passed.

```typescript
public static deserialize(serialized: Uint8Array, portable: boolean): RoaringBitmap32;
```

**Parameters**

| Name       | Type       | Description                                                                             |
| ---------- | ---------- | --------------------------------------------------------------------------------------- |
| serialized | Uint8Array | An Uint8Array or a node Buffer that contains the serialized data.                       |
| portable   | boolean    | If false, optimized C/C++ format is used. If true, Java and Go portable format is used. |

**Return type**

[RoaringBitmap32][classdeclaration-0]

---

### deserialize(serialized, portable)

Deserializes the bitmap from an Uint8Array or a Buffer.

Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
The portable version is meant to be compatible with Java and Go versions.

```typescript
public deserialize(serialized: Uint8Array, portable: boolean): RoaringBitmap32;
```

**Parameters**

| Name       | Type       | Description                                                                             |
| ---------- | ---------- | --------------------------------------------------------------------------------------- |
| serialized | Uint8Array | An Uint8Array or a node Buffer that contains the serialized data.                       |
| portable   | boolean    | If false, optimized C/C++ format is used. If true, Java and Go portable format is used. |

**Return type**

[RoaringBitmap32][classdeclaration-0]

---

### deserializeAsync(serialized, portable)

Deserializes the bitmap from an Uint8Array or a Buffer asynchrnously in a parallel thread.

Returns a Promise that resolves to a new RoaringBitmap32 instance.

Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
The portable version is meant to be compatible with Java and Go versions.

NOTE: portable argument was optional before, now is required and an Error is thrown if the portable flag is not passed.
Deserializes the bitmap from an Uint8Array or a Buffer asynchrnously in a parallel thread.

When deserialization is completed or failed, the given callback will be executed.

Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
The portable version is meant to be compatible with Java and Go versions.

NOTE: portable argument was optional before, now is required and an Error is thrown if the portable flag is not passed.

```typescript
public static deserializeAsync(serialized: Uint8Array, portable: boolean): Promise<RoaringBitmap32>;
```

**Parameters**

| Name       | Type       | Description                                                                             |
| ---------- | ---------- | --------------------------------------------------------------------------------------- |
| serialized | Uint8Array | An Uint8Array or a node Buffer that contains the serialized data.                       |
| portable   | boolean    | If false, optimized C/C++ format is used. If true, Java and Go portable format is used. |

**Return type**

Promise<[RoaringBitmap32][classdeclaration-0]>

---

### deserializeAsync(serialized, portable, callback)

Deserializes the bitmap from an Uint8Array or a Buffer asynchrnously in a parallel thread.

Returns a Promise that resolves to a new RoaringBitmap32 instance.

Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
The portable version is meant to be compatible with Java and Go versions.

NOTE: portable argument was optional before, now is required and an Error is thrown if the portable flag is not passed.
Deserializes the bitmap from an Uint8Array or a Buffer asynchrnously in a parallel thread.

When deserialization is completed or failed, the given callback will be executed.

Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
The portable version is meant to be compatible with Java and Go versions.

NOTE: portable argument was optional before, now is required and an Error is thrown if the portable flag is not passed.

```typescript
public static deserializeAsync(serialized: Uint8Array, portable: boolean, callback: RoaringBitmap32Callback): void;
```

**Parameters**

| Name       | Type                                              | Description                                                                             |
| ---------- | ------------------------------------------------- | --------------------------------------------------------------------------------------- |
| serialized | Uint8Array                                        | An Uint8Array or a node Buffer that contains the.                                       |
| portable   | boolean                                           | If false, optimized C/C++ format is used. If true, Java and Go portable format is used. |
| callback   | [RoaringBitmap32Callback][typealiasdeclaration-0] | The callback to execute when the operation completes.                                   |

**Return type**

void

---

### deserializeParallelAsync(serialized, portable)

Deserializes many bitmaps from an array of Uint8Array or an array of Buffer asynchronously in multiple parallel threads.

Returns a Promise that resolves to an array of new RoaringBitmap32 instance.

Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
The portable version is meant to be compatible with Java and Go versions.

NOTE: portable argument was optional before, now is required and an Error is thrown if the portable flag is not passed.
Deserializes many bitmaps from an array of Uint8Array or an array of Buffer asynchronously in a parallel thread.

Deserialization in the parallel thread will be executed in sequence, if one fails, all fails.

Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
The portable version is meant to be compatible with Java and Go versions.

NOTE: portable argument was optional before, now is required and an Error is thrown if the portable flag is not passed.

When deserialization is completed or failed, the given callback will be executed.

```typescript
public static deserializeParallelAsync(serialized: (Uint8Array | null | undefined)[], portable: boolean): Promise<RoaringBitmap32[]>;
```

**Parameters**

| Name       | Type                                        | Description                                                                             |
| ---------- | ------------------------------------------- | --------------------------------------------------------------------------------------- |
| serialized | (Uint8Array &#124; null &#124; undefined)[] | An Uint8Array or a node Buffer that contains the serialized data.                       |
| portable   | boolean                                     | If false, optimized C/C++ format is used. If true, Java and Go portable format is used. |

**Return type**

Promise<[RoaringBitmap32][classdeclaration-0][]>

---

### deserializeParallelAsync(serialized, portable, callback)

Deserializes many bitmaps from an array of Uint8Array or an array of Buffer asynchronously in multiple parallel threads.

Returns a Promise that resolves to an array of new RoaringBitmap32 instance.

Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
The portable version is meant to be compatible with Java and Go versions.

NOTE: portable argument was optional before, now is required and an Error is thrown if the portable flag is not passed.
Deserializes many bitmaps from an array of Uint8Array or an array of Buffer asynchronously in a parallel thread.

Deserialization in the parallel thread will be executed in sequence, if one fails, all fails.

Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
The portable version is meant to be compatible with Java and Go versions.

NOTE: portable argument was optional before, now is required and an Error is thrown if the portable flag is not passed.

When deserialization is completed or failed, the given callback will be executed.

```typescript
public static deserializeParallelAsync(serialized: (Uint8Array | null | undefined)[], portable: boolean, callback: RoaringBitmap32ArrayCallback): void;
```

**Parameters**

| Name       | Type                                                   | Description                                                                            |
| ---------- | ------------------------------------------------------ | -------------------------------------------------------------------------------------- |
| serialized | (Uint8Array &#124; null &#124; undefined)[]            | An array of Uint8Array or node Buffers that contains the non portable serialized data. |
| portable   | boolean                                                |                                                                                        |
| callback   | [RoaringBitmap32ArrayCallback][typealiasdeclaration-1] | The callback to execute when the operation completes.                                  |

**Return type**

void

---

### swap(a, b)

Swaps the content of two RoaringBitmap32 instances.

```typescript
public static swap(a: RoaringBitmap32, b: RoaringBitmap32): void;
```

**Parameters**

| Name | Type                                  | Description                             |
| ---- | ------------------------------------- | --------------------------------------- |
| a    | [RoaringBitmap32][classdeclaration-0] | First RoaringBitmap32 instance to swap  |
| b    | [RoaringBitmap32][classdeclaration-0] | Second RoaringBitmap32 instance to swap |

**Return type**

void

---

### and(a, b)

Returns a new RoaringBitmap32 with the intersection (and) between the given two bitmaps.

The provided bitmaps are not modified.

```typescript
public static and(a: RoaringBitmap32, b: RoaringBitmap32): RoaringBitmap32;
```

**Parameters**

| Name | Type                                  | Description                                 |
| ---- | ------------------------------------- | ------------------------------------------- |
| a    | [RoaringBitmap32][classdeclaration-0] | The first RoaringBitmap32 instance to and.  |
| b    | [RoaringBitmap32][classdeclaration-0] | The second RoaringBitmap32 instance to and. |

**Return type**

[RoaringBitmap32][classdeclaration-0]

---

### or(a, b)

Returns a new RoaringBitmap32 with the union (or) of the two given bitmaps.

The provided bitmaps are not modified.

```typescript
public static or(a: RoaringBitmap32, b: RoaringBitmap32): RoaringBitmap32;
```

**Parameters**

| Name | Type                                  | Description                                |
| ---- | ------------------------------------- | ------------------------------------------ |
| a    | [RoaringBitmap32][classdeclaration-0] | The first RoaringBitmap32 instance to or.  |
| b    | [RoaringBitmap32][classdeclaration-0] | The second RoaringBitmap32 instance to or. |

**Return type**

[RoaringBitmap32][classdeclaration-0]

---

### xor(a, b)

Returns a new RoaringBitmap32 with the symmetric union (xor) between the two given bitmaps.

The provided bitmaps are not modified.

```typescript
public static xor(a: RoaringBitmap32, b: RoaringBitmap32): RoaringBitmap32;
```

**Parameters**

| Name | Type                                  | Description                                 |
| ---- | ------------------------------------- | ------------------------------------------- |
| a    | [RoaringBitmap32][classdeclaration-0] | The first RoaringBitmap32 instance to xor.  |
| b    | [RoaringBitmap32][classdeclaration-0] | The second RoaringBitmap32 instance to xor. |

**Return type**

[RoaringBitmap32][classdeclaration-0]

---

### andNot(a, b)

Returns a new RoaringBitmap32 with the difference (and not) between the two given bitmaps.

The provided bitmaps are not modified.

```typescript
public static andNot(a: RoaringBitmap32, b: RoaringBitmap32): RoaringBitmap32;
```

**Parameters**

| Name | Type                                  | Description                          |
| ---- | ------------------------------------- | ------------------------------------ |
| a    | [RoaringBitmap32][classdeclaration-0] | The first RoaringBitmap32 instance.  |
| b    | [RoaringBitmap32][classdeclaration-0] | The second RoaringBitmap32 instance. |

**Return type**

[RoaringBitmap32][classdeclaration-0]

---

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
| values | [RoaringBitmap32][classdeclaration-0][] | An array of RoaringBitmap32 instances to or together. |

**Return type**

[RoaringBitmap32][classdeclaration-0]

---

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
| values | [RoaringBitmap32][classdeclaration-0][] | The RoaringBitmap32 instances to or together. |

**Return type**

[RoaringBitmap32][classdeclaration-0]

---

### xorMany(values)

Performs a xor between all the given array of RoaringBitmap32 instances.

This function is faster than calling xor multiple times.
Performs a xor between all the given RoaringBitmap32 instances.

This function is faster than calling xor multiple times.

```typescript
public static xorMany(values: RoaringBitmap32[]): RoaringBitmap32;
```

**Parameters**

| Name   | Type                                    | Description                                           |
| ------ | --------------------------------------- | ----------------------------------------------------- |
| values | [RoaringBitmap32][classdeclaration-0][] | An array of RoaringBitmap32 instances to or together. |

**Return type**

[RoaringBitmap32][classdeclaration-0]

---

### xorMany(values)

Performs a xor between all the given array of RoaringBitmap32 instances.

This function is faster than calling xor multiple times.
Performs a xor between all the given RoaringBitmap32 instances.

This function is faster than calling xor multiple times.

```typescript
public static xorMany(values: RoaringBitmap32[]): RoaringBitmap32;
```

**Parameters**

| Name   | Type                                    | Description                                   |
| ------ | --------------------------------------- | --------------------------------------------- |
| values | [RoaringBitmap32][classdeclaration-0][] | The RoaringBitmap32 instances to or together. |

**Return type**

[RoaringBitmap32][classdeclaration-0]

---

### \_\_@iterator()

[Symbol.iterator]() Gets a new iterator able to iterate all values in the set in ascending order.

WARNING: Is not allowed to change the bitmap while iterating.
The iterator may throw exception if the bitmap is changed during the iteration.

```typescript
public __@iterator(): RoaringBitmap32Iterator;
```

**Return type**

[RoaringBitmap32Iterator][classdeclaration-1]

---

### iterator()

Gets a new iterator able to iterate all values in the set in ascending order.

WARNING: Is not allowed to change the bitmap while iterating.
The iterator may throw exception if the bitmap is changed during the iteration.

Same as [Symbol.iterator]()

```typescript
public iterator(): RoaringBitmap32Iterator;
```

**Return type**

[RoaringBitmap32Iterator][classdeclaration-1]

---

### keys()

Gets a new iterator able to iterate all values in the set in ascending order.
This is just for compatibility with the Set<number> interface.

WARNING: Is not allowed to change the bitmap while iterating. Undefined behaviour.
The iterator may throw exception if the bitmap is changed during the iteration.

Same as [Symbol.iterator]()

```typescript
public keys(): RoaringBitmap32Iterator;
```

**Return type**

[RoaringBitmap32Iterator][classdeclaration-1]

---

### values()

Gets a new iterator able to iterate all values in the set in ascending order.
This is just for compatibility with the Set<number> interface.

WARNING: Is not allowed to change the bitmap while iterating. Undefined behaviour.
The iterator may throw exception if the bitmap is changed during the iteration.

Same as [Symbol.iterator]()

```typescript
public values(): RoaringBitmap32Iterator;
```

**Return type**

[RoaringBitmap32Iterator][classdeclaration-1]

---

### entries()

Gets a new iterator able to iterate all value pairs [value, value] in the set in ascending order.
This is just for compatibility with the Set<number> interface.

WARNING: Is not allowed to change the bitmap while iterating. Undefined behaviour.
The iterator may throw exception if the bitmap is changed during the iteration.

Same as [Symbol.iterator]()

```typescript
public entries(): IterableIterator<[number, number]>;
```

**Return type**

IterableIterator<[number, number]>

---

### forEach(callbackfn, thisArg)

Executes a function for each value in the set, in ascending order.
The callback has 3 arguments, the value, the value and this (this set). This is to match the Set<number> interface.

WARNING: Is not allowed to change the bitmap while iterating. Undefined behaviour.

```typescript
public forEach(callbackfn: (value: number, value2: number, set: RoaringBitmap32) => void, thisArg?: any): void;
```

**Parameters**

| Name       | Type                                                          |
| ---------- | ------------------------------------------------------------- |
| callbackfn | (value: number, value2: number, set: RoaringBitmap32) => void |
| thisArg    | any                                                           |

**Return type**

void

---

### minimum()

Gets the minimum value in the set.

```typescript
public minimum(): number;
```

**Return type**

number

---

### maximum()

Gets the maximum value in the set.

```typescript
public maximum(): number;
```

**Return type**

number

---

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

---

### hasRange(rangeStart, rangeEnd)

Check whether a range of values from rangeStart (included) to rangeEnd (excluded) is present

```typescript
public hasRange(rangeStart: number, rangeEnd: number): boolean;
```

**Parameters**

| Name       | Type   | Description                  |
| ---------- | ------ | ---------------------------- |
| rangeStart | number | The start index (inclusive). |
| rangeEnd   | number | The end index (exclusive).   |

**Return type**

boolean

---

### rangeCardinality(rangeStart, rangeEnd)

Gets the cardinality (number of elements) between rangeStart (included) to rangeEnd (excluded) of the bitmap.
Returns 0 if range is invalid or if no element was found in the given range.

```typescript
public rangeCardinality(rangeStart: number, rangeEnd: number): number;
```

**Parameters**

| Name       | Type   | Description                  |
| ---------- | ------ | ---------------------------- |
| rangeStart | number | The start index (inclusive). |
| rangeEnd   | number | The end index (exclusive).   |

**Return type**

number

---

### copyFrom(values)

Overwrite the content of this bitmap copying it from an Iterable or another RoaringBitmap32.

Is faster to pass a Uint32Array instance instead of an array or an iterable.

Is even faster if a RoaringBitmap32 instance is used (it performs a simple copy).

```typescript
public copyFrom(values: Iterable<number> | null | undefined): RoaringBitmap32;
```

**Parameters**

| Name   | Type                                          | Description                                   |
| ------ | --------------------------------------------- | --------------------------------------------- |
| values | Iterable<number> &#124; null &#124; undefined | The new values or a RoaringBitmap32 instance. |

**Return type**

[RoaringBitmap32][classdeclaration-0]

---

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

[RoaringBitmap32][classdeclaration-0]

---

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

---

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

[RoaringBitmap32][classdeclaration-0]

---

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

---

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

---

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

[RoaringBitmap32][classdeclaration-0]

---

### flipRange(rangeStart, rangeEnd)

Negates (in place) the roaring bitmap within a specified interval: [rangeStart, rangeEnd).

First element is included, last element is excluded.
The number of negated values is rangeEnd - rangeStart.

Areas outside the range are passed through unchanged.

```typescript
public flipRange(rangeStart: number, rangeEnd: number): RoaringBitmap32;
```

**Parameters**

| Name       | Type   | Description                           |
| ---------- | ------ | ------------------------------------- |
| rangeStart | number | The start index. Trimmed to 0.        |
| rangeEnd   | number | The end index. Trimmed to 4294967297. |

**Return type**

[RoaringBitmap32][classdeclaration-0]

---

### addRange(rangeStart, rangeEnd)

Adds all the values in the interval: [rangeStart, rangeEnd).

First element is included, last element is excluded.
The number of added values is rangeEnd - rangeStart.

Areas outside the range are passed through unchanged.

```typescript
public addRange(rangeStart: number, rangeEnd: number): RoaringBitmap32;
```

**Parameters**

| Name       | Type   | Description                           |
| ---------- | ------ | ------------------------------------- |
| rangeStart | number | The start index. Trimmed to 0.        |
| rangeEnd   | number | The end index. Trimmed to 4294967297. |

**Return type**

[RoaringBitmap32][classdeclaration-0]

---

### removeRange(rangeStart, rangeEnd)

Removes all the values in the interval: [rangeStart, rangeEnd).

First element is included, last element is excluded.
The number of renived values is rangeEnd - rangeStart.

Areas outside the range are passed through unchanged.

```typescript
public removeRange(rangeStart: number, rangeEnd: number): RoaringBitmap32;
```

**Parameters**

| Name       | Type   | Description                           |
| ---------- | ------ | ------------------------------------- |
| rangeStart | number | The start index. Trimmed to 0.        |
| rangeEnd   | number | The end index. Trimmed to 4294967297. |

**Return type**

[RoaringBitmap32][classdeclaration-0]

---

### clear()

Removes all values from the set.

It frees resources, you can use clear() to free some memory before the garbage collector disposes this instance.

```typescript
public clear(): boolean;
```

**Return type**

boolean

---

### orInPlace(values)

Performs an union in place ("this = this OR values"), same as addMany.

Is faster to use Uint32Array instead of arrays or iterables.

This function is optimized if the argument is an instance of RoaringBitmap32.

```typescript
public orInPlace(values: Iterable<number>): RoaringBitmap32;
```

**Parameters**

| Name   | Type             | Description                                                            |
| ------ | ---------------- | ---------------------------------------------------------------------- |
| values | Iterable<number> | A RoaringBitmap32 instance or an iterable of unsigned 32 bit integers. |

**Return type**

[RoaringBitmap32][classdeclaration-0]

---

### andNotInPlace(values)

Performs a AND NOT operation in place ("this = this AND NOT values"), same as removeMany.

Is faster to use Uint32Array instead of arrays or iterables.

This function is optimized if the argument is an instance of RoaringBitmap32.

```typescript
public andNotInPlace(values: Iterable<number>): RoaringBitmap32;
```

**Parameters**

| Name   | Type             | Description                                                            |
| ------ | ---------------- | ---------------------------------------------------------------------- |
| values | Iterable<number> | A RoaringBitmap32 instance or an iterable of unsigned 32 bit integers. |

**Return type**

[RoaringBitmap32][classdeclaration-0]

---

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

[RoaringBitmap32][classdeclaration-0]

---

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

[RoaringBitmap32][classdeclaration-0]

---

### isSubset(other)

Checks wether this set is a subset or the same as the given set.

Returns false also if the given argument is not a RoaringBitmap32 instance.

```typescript
public isSubset(other: RoaringBitmap32): boolean;
```

**Parameters**

| Name  | Type                                  | Description    |
| ----- | ------------------------------------- | -------------- |
| other | [RoaringBitmap32][classdeclaration-0] | The other set. |

**Return type**

boolean

---

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
| other | [RoaringBitmap32][classdeclaration-0] | The other RoaringBitmap32 instance. |

**Return type**

boolean

---

### isEqual(other)

Checks wether this set is equal to another set.

Returns false also if the given argument is not a RoaringBitmap32 instance.

```typescript
public isEqual(other: RoaringBitmap32): boolean;
```

**Parameters**

| Name  | Type                                  | Description                            |
| ----- | ------------------------------------- | -------------------------------------- |
| other | [RoaringBitmap32][classdeclaration-0] | The other set to compare for equality. |

**Return type**

boolean

---

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
| other | [RoaringBitmap32][classdeclaration-0] | The other set to compare for intersection. |

**Return type**

boolean

---

### andCardinality(other)

Computes the size of the intersection between two bitmaps (the number of values in common).

Returns -1 if the given argument is not a RoaringBitmap32 instance.

```typescript
public andCardinality(other: RoaringBitmap32): number;
```

**Parameters**

| Name  | Type                                  | Description                                |
| ----- | ------------------------------------- | ------------------------------------------ |
| other | [RoaringBitmap32][classdeclaration-0] | The other set to compare for intersection. |

**Return type**

number

---

### orCardinality(other)

Computes the size of the union between two bitmaps.

Returns -1 if the given argument is not a RoaringBitmap32 instance.

```typescript
public orCardinality(other: RoaringBitmap32): number;
```

**Parameters**

| Name  | Type                                  | Description                                |
| ----- | ------------------------------------- | ------------------------------------------ |
| other | [RoaringBitmap32][classdeclaration-0] | The other set to compare for intersection. |

**Return type**

number

---

### andNotCardinality(other)

Computes the size of the difference (andnot) between two bitmaps.

Returns -1 if the given argument is not a RoaringBitmap32 instance.

```typescript
public andNotCardinality(other: RoaringBitmap32): number;
```

**Parameters**

| Name  | Type                                  | Description                                |
| ----- | ------------------------------------- | ------------------------------------------ |
| other | [RoaringBitmap32][classdeclaration-0] | The other set to compare for intersection. |

**Return type**

number

---

### xorCardinality(other)

Computes the size of the symmetric difference (xor) between two bitmaps.

Returns -1 if the given argument is not a RoaringBitmap32 instance.

```typescript
public xorCardinality(other: RoaringBitmap32): number;
```

**Parameters**

| Name  | Type                                  | Description                                |
| ----- | ------------------------------------- | ------------------------------------------ |
| other | [RoaringBitmap32][classdeclaration-0] | The other set to compare for intersection. |

**Return type**

number

---

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
| other | [RoaringBitmap32][classdeclaration-0] | The other RoaringBitmap32 to compare. |

**Return type**

number

---

### removeRunCompression()

Remove run-length encoding even when it is more space efficient.

Return whether a change was applied.

```typescript
public removeRunCompression(): boolean;
```

**Return type**

boolean

---

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

---

### shrinkToFit()

If needed, reallocate memory to shrink the memory usage.

Returns the number of bytes saved.

```typescript
public shrinkToFit(): number;
```

**Return type**

number

---

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

---

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

---

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

---

### rangeUint32Array(offset, limit)

to array with pagination

```typescript
public rangeUint32Array(offset: number, limit: number): Uint32Array;
```

**Parameters**

| Name   | Type   |
| ------ | ------ |
| offset | number |
| limit  | number |

**Return type**

Uint32Array

---

### toArray()

Creates a new plain JS array and fills it with all the values in the bitmap.

The returned array may be very big, use this function only when you know what you are doing.

```typescript
public toArray(): number[];
```

**Return type**

number[]

---

### toSet()

Creates a new plain JS Set<number> and fills it with all the values in the bitmap.

The returned set may be very big, use this function only when you know what you are doing.

```typescript
public toSet(): Set<number>;
```

**Return type**

Set<number>

---

### toJSON()

Returns a plain JS array with all the values in the bitmap.

Used by JSON.stringify to serialize this bitmap as an array.

```typescript
public toJSON(): number[];
```

**Return type**

number[]

---

### getSerializationSizeInBytes(portable)

How many bytes are required to serialize this bitmap.

Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
The portable version is meant to be compatible with Java and Go versions.

NOTE: portable argument was optional before, now is required and an Error is thrown if the portable flag is not passed.

```typescript
public getSerializationSizeInBytes(portable: boolean): number;
```

**Parameters**

| Name     | Type    | Description                                                                             |
| -------- | ------- | --------------------------------------------------------------------------------------- |
| portable | boolean | If false, optimized C/C++ format is used. If true, Java and Go portable format is used. |

**Return type**

number

---

### serialize(portable)

Serializes the bitmap into a new Buffer.

Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
The portable version is meant to be compatible with Java and Go versions.

NOTE: portable argument was optional before, now is required and an Error is thrown if the portable flag is not passed.

```typescript
public serialize(portable: boolean): Buffer;
```

**Parameters**

| Name     | Type    | Description                                                                             |
| -------- | ------- | --------------------------------------------------------------------------------------- |
| portable | boolean | If false, optimized C/C++ format is used. If true, Java and Go portable format is used. |

**Return type**

[Buffer][interfacedeclaration-0]

---

### clone()

Returns a new RoaringBitmap32 that is a copy of this bitmap, same as new RoaringBitmap32(copy)

```typescript
public clone(): RoaringBitmap32;
```

**Return type**

[RoaringBitmap32][classdeclaration-0]

---

### toString()

Returns always "RoaringBitmap32".

To have a standard string representation of the content as a string, call contentToString() instead.

```typescript
public toString(): string;
```

**Return type**

string

---

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

---

### statistics()

Returns an object that contains statistic information about this RoaringBitmap32 instance.

```typescript
public statistics(): RoaringBitmap32Statistics;
```

**Return type**

[RoaringBitmap32Statistics][interfacedeclaration-1]

## Properties

### CRoaringVersion

Property: The version of the CRoaring libary as a string.
Example: "0.4.0"

```typescript
public static readonly CRoaringVersion: string;
```

**Type**

string

---

### CRoaringVersion

Property: The version of the CRoaring libary as a string.
Example: "0.4.0"

```typescript
public readonly CRoaringVersion: string;
```

**Type**

string

---

### PackageVersion

Property: The version of the roaring npm package as a string.
Example: "1.2.0"

```typescript
public static readonly PackageVersion: string;
```

**Type**

string

---

### PackageVersion

Property: The version of the roaring npm package as a string.
Example: "1.2.0"

```typescript
public readonly PackageVersion: string;
```

**Type**

string

---

### size

Property. Gets the number of items in the set (cardinality).

```typescript
public readonly size: number;
```

**Type**

number

---

### isEmpty

Property. True if the bitmap is empty.

```typescript
public readonly isEmpty: boolean;
```

**Type**

boolean

---

### \_\_@toStringTag

Returns always "RoaringBitmap32".

To have a standard string representation of the content as a string, call contentToString() instead.

```typescript
public readonly __@toStringTag: "Set";
```

**Type**

"Set"

[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[constructor-0]: roaringbitmap32.md#constructorvalues
[methoddeclaration-0]: roaringbitmap32.md#fromvalues
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-1]: roaringbitmap32.md#fromrangerangestart-rangeend-step
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-2]: roaringbitmap32.md#fromarrayasyncvalues
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-3]: roaringbitmap32.md#fromarrayasyncvalues-callback
[typealiasdeclaration-0]: ../index.d.md#roaringbitmap32callback
[methoddeclaration-4]: roaringbitmap32.md#deserializeserialized-portable
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-67]: roaringbitmap32.md#deserializeserialized-portable
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-5]: roaringbitmap32.md#deserializeasyncserialized-portable
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-6]: roaringbitmap32.md#deserializeasyncserialized-portable-callback
[typealiasdeclaration-0]: ../index.d.md#roaringbitmap32callback
[methoddeclaration-7]: roaringbitmap32.md#deserializeparallelasyncserialized-portable
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-8]: roaringbitmap32.md#deserializeparallelasyncserialized-portable-callback
[typealiasdeclaration-1]: ../index.d.md#roaringbitmap32arraycallback
[methoddeclaration-9]: roaringbitmap32.md#swapa-b
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-10]: roaringbitmap32.md#anda-b
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-11]: roaringbitmap32.md#ora-b
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-12]: roaringbitmap32.md#xora-b
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-13]: roaringbitmap32.md#andnota-b
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-14]: roaringbitmap32.md#ormanyvalues
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-15]: roaringbitmap32.md#ormanyvalues
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-16]: roaringbitmap32.md#xormanyvalues
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-17]: roaringbitmap32.md#xormanyvalues
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-18]: roaringbitmap32.md#__iterator
[classdeclaration-1]: roaringbitmap32iterator.md#roaringbitmap32iterator
[methoddeclaration-21]: roaringbitmap32.md#iterator
[classdeclaration-1]: roaringbitmap32iterator.md#roaringbitmap32iterator
[methoddeclaration-22]: roaringbitmap32.md#keys
[classdeclaration-1]: roaringbitmap32iterator.md#roaringbitmap32iterator
[methoddeclaration-23]: roaringbitmap32.md#values
[classdeclaration-1]: roaringbitmap32iterator.md#roaringbitmap32iterator
[methoddeclaration-24]: roaringbitmap32.md#entries
[methoddeclaration-25]: roaringbitmap32.md#foreachcallbackfn-thisarg
[methoddeclaration-26]: roaringbitmap32.md#minimum
[methoddeclaration-27]: roaringbitmap32.md#maximum
[methoddeclaration-28]: roaringbitmap32.md#hasvalue
[methoddeclaration-29]: roaringbitmap32.md#hasrangerangestart-rangeend
[methoddeclaration-30]: roaringbitmap32.md#rangecardinalityrangestart-rangeend
[methoddeclaration-31]: roaringbitmap32.md#copyfromvalues
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-32]: roaringbitmap32.md#addvalue
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-33]: roaringbitmap32.md#tryaddvalue
[methoddeclaration-34]: roaringbitmap32.md#addmanyvalues
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-35]: roaringbitmap32.md#deletevalue
[methoddeclaration-36]: roaringbitmap32.md#removevalue
[methoddeclaration-37]: roaringbitmap32.md#removemanyvalues
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-38]: roaringbitmap32.md#fliprangerangestart-rangeend
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-39]: roaringbitmap32.md#addrangerangestart-rangeend
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-40]: roaringbitmap32.md#removerangerangestart-rangeend
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-41]: roaringbitmap32.md#clear
[methoddeclaration-42]: roaringbitmap32.md#orinplacevalues
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-43]: roaringbitmap32.md#andnotinplacevalues
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-44]: roaringbitmap32.md#andinplacevalues
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-45]: roaringbitmap32.md#xorinplacevalues
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-46]: roaringbitmap32.md#issubsetother
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-47]: roaringbitmap32.md#isstrictsubsetother
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-48]: roaringbitmap32.md#isequalother
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-49]: roaringbitmap32.md#intersectsother
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-50]: roaringbitmap32.md#andcardinalityother
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-51]: roaringbitmap32.md#orcardinalityother
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-52]: roaringbitmap32.md#andnotcardinalityother
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-53]: roaringbitmap32.md#xorcardinalityother
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-54]: roaringbitmap32.md#jaccardindexother
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-55]: roaringbitmap32.md#removeruncompression
[methoddeclaration-56]: roaringbitmap32.md#runoptimize
[methoddeclaration-57]: roaringbitmap32.md#shrinktofit
[methoddeclaration-58]: roaringbitmap32.md#rankmaxvalue
[methoddeclaration-59]: roaringbitmap32.md#selectrank
[methoddeclaration-60]: roaringbitmap32.md#touint32array
[methoddeclaration-61]: roaringbitmap32.md#rangeuint32arrayoffset-limit
[methoddeclaration-62]: roaringbitmap32.md#toarray
[methoddeclaration-63]: roaringbitmap32.md#toset
[methoddeclaration-64]: roaringbitmap32.md#tojson
[methoddeclaration-65]: roaringbitmap32.md#getserializationsizeinbytesportable
[methoddeclaration-66]: roaringbitmap32.md#serializeportable
[interfacedeclaration-0]: ../index.d.md#indexdts
[methoddeclaration-68]: roaringbitmap32.md#clone
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-69]: roaringbitmap32.md#tostring
[methoddeclaration-70]: roaringbitmap32.md#contenttostringmaxlength
[methoddeclaration-71]: roaringbitmap32.md#statistics
[interfacedeclaration-1]: ../index.d.md#roaringbitmap32statistics
[propertydeclaration-0]: roaringbitmap32.md#croaringversion
[propertydeclaration-2]: roaringbitmap32.md#croaringversion
[propertydeclaration-1]: roaringbitmap32.md#packageversion
[propertydeclaration-3]: roaringbitmap32.md#packageversion
[propertydeclaration-4]: roaringbitmap32.md#size
[propertydeclaration-5]: roaringbitmap32.md#isempty
[propertydeclaration-6]: roaringbitmap32.md#__tostringtag
