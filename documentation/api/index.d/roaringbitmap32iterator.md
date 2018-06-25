# Table of contents

* [RoaringBitmap32Iterator][ClassDeclaration-1]
    * Constructor
        * [constructor(roaringBitmap32)][Constructor-1]
        * [constructor(roaringBitmap32, bufferSize)][Constructor-2]
        * [constructor(roaringBitmap32, buffer)][Constructor-3]
    * Methods
        * [__@iterator()][MethodDeclaration-18]
        * [next()][MethodDeclaration-19]

# RoaringBitmap32Iterator

Iterator for RoaringBitmap32

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

----------

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
| roaringBitmap32 | [RoaringBitmap32][ClassDeclaration-0] | The roaring bitmap to iterate                              |
| bufferSize      | number                                | Buffer size to allocate, must be an integer greater than 0 |

----------

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
| roaringBitmap32 | [RoaringBitmap32][ClassDeclaration-0] |                               |
| buffer          | Uint32Array                           | The roaring bitmap to iterate |

## Methods

### __@iterator()

Returns this.

```typescript
public __@iterator(): RoaringBitmap32Iterator;
```

**Return type**

[RoaringBitmap32Iterator][ClassDeclaration-1]

----------

### next()

Returns the next element in the iterator.

For performance reasons, this function returns always the same instance.

```typescript
public next(): IteratorResult<number>;
```

**Return type**

IteratorResult<number>

[ClassDeclaration-1]: roaringbitmap32iterator.md#roaringbitmap32iterator
[Constructor-1]: roaringbitmap32iterator.md#constructorroaringbitmap32
[Constructor-2]: roaringbitmap32iterator.md#constructorroaringbitmap32-buffersize
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[Constructor-3]: roaringbitmap32iterator.md#constructorroaringbitmap32-buffer
[ClassDeclaration-0]: roaringbitmap32.md#roaringbitmap32
[MethodDeclaration-18]: roaringbitmap32iterator.md#__iterator
[ClassDeclaration-1]: roaringbitmap32iterator.md#roaringbitmap32iterator
[MethodDeclaration-19]: roaringbitmap32iterator.md#next