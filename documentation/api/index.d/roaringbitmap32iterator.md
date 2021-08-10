# Table of contents

- [RoaringBitmap32Iterator][classdeclaration-1]
  - Constructor
    - [constructor(roaringBitmap32)][constructor-1]
    - [constructor(roaringBitmap32, bufferSize)][constructor-2]
    - [constructor(roaringBitmap32, buffer)][constructor-3]
  - Methods
    - [\_\_@iterator()][methoddeclaration-19]
    - [next()][methoddeclaration-20]

# RoaringBitmap32Iterator

Iterator for RoaringBitmap32.

WARNING: Is not allowed to change the bitmap while iterating.
The iterator may throw exception if the bitmap is changed during the iteration.

```typescript
class RoaringBitmap32Iterator implements IterableIterator<number>
```

## Constructor

### constructor(roaringBitmap32)

Creates a new iterator able to iterate a RoaringBitmap32.
Creates a new iterator able to iterate a RoaringBitmap32.

It allocates a small temporary buffer of the given size for speedup.
Creates a new iterator able to iterate a RoaringBitmap32 using the given temporary buffer.

```typescript
public constructor(roaringBitmap32?: RoaringBitmap32 | undefined);
```

**Parameters**

| Name            | Type                             | Description                                                                        |
| --------------- | -------------------------------- | ---------------------------------------------------------------------------------- |
| roaringBitmap32 | RoaringBitmap32 &#124; undefined | The roaring bitmap to iterate. If null or undefined, an empty iterator is created. |

---

### constructor(roaringBitmap32, bufferSize)

Creates a new iterator able to iterate a RoaringBitmap32.
Creates a new iterator able to iterate a RoaringBitmap32.

It allocates a small temporary buffer of the given size for speedup.
Creates a new iterator able to iterate a RoaringBitmap32 using the given temporary buffer.

```typescript
public constructor(roaringBitmap32: RoaringBitmap32, bufferSize: number);
```

**Parameters**

| Name            | Type                                  | Description                                                |
| --------------- | ------------------------------------- | ---------------------------------------------------------- |
| roaringBitmap32 | [RoaringBitmap32][classdeclaration-0] | The roaring bitmap to iterate                              |
| bufferSize      | number                                | Buffer size to allocate, must be an integer greater than 0 |

---

### constructor(roaringBitmap32, buffer)

Creates a new iterator able to iterate a RoaringBitmap32.
Creates a new iterator able to iterate a RoaringBitmap32.

It allocates a small temporary buffer of the given size for speedup.
Creates a new iterator able to iterate a RoaringBitmap32 using the given temporary buffer.

```typescript
public constructor(roaringBitmap32: RoaringBitmap32, buffer: Uint32Array);
```

**Parameters**

| Name            | Type                                  | Description                   |
| --------------- | ------------------------------------- | ----------------------------- |
| roaringBitmap32 | [RoaringBitmap32][classdeclaration-0] |                               |
| buffer          | Uint32Array                           | The roaring bitmap to iterate |

## Methods

### \_\_@iterator()

Returns this.

```typescript
public __@iterator(): RoaringBitmap32Iterator;
```

**Return type**

[RoaringBitmap32Iterator][classdeclaration-1]

---

### next()

Returns the next element in the iterator.

For performance reasons, this function returns always the same instance.

```typescript
public next(): IteratorResult<number>;
```

**Return type**

IteratorResult<number>

[classdeclaration-1]: roaringbitmap32iterator.md#roaringbitmap32iterator
[constructor-1]: roaringbitmap32iterator.md#constructorroaringbitmap32
[constructor-2]: roaringbitmap32iterator.md#constructorroaringbitmap32-buffersize
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[constructor-3]: roaringbitmap32iterator.md#constructorroaringbitmap32-buffer
[classdeclaration-0]: roaringbitmap32.md#roaringbitmap32
[methoddeclaration-19]: roaringbitmap32iterator.md#__iterator
[classdeclaration-1]: roaringbitmap32iterator.md#roaringbitmap32iterator
[methoddeclaration-20]: roaringbitmap32iterator.md#next
