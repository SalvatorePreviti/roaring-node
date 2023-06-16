/*
Copyright 2018 Salvatore Previti

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

Source code at: https://github.com/SalvatorePreviti/roaring-node 

Documentation at: https://salvatorepreviti.github.io/roaring-node/modules.html

Roaring Bitmap 32 documentation at: https://salvatorepreviti.github.io/roaring-node/classes/RoaringBitmap32.html

*/

import roaring = require("./");

/** Gets the approximate memory allocated by the roaring bitmap library. */
export function getRoaringUsedMemory(): number;

/**
 * Creates a new buffer with the given size and alignment.
 * If alignment is not specified, the default alignment of 32 is used.
 * The buffer does not come from the nodejs buffer pool, it is allocated using aligned_malloc.
 *
 * Is the same as Buffer.alloc but is aligned.
 * We need an aligned buffer to create a roaring bitmap frozen view.
 *
 * @param {number} size The size of the buffer to allocate.
 * @param {number} [alignment=32] The alignment of the buffer to allocate.
 */
export function bufferAlignedAlloc(size: number, alignment?: number): Buffer;

/**
 * Creates a new buffer not initialized with the given size and alignment.
 * If alignment is not specified, the default alignment of 32 is used.
 * The buffer does not come from the nodejs buffer pool, it is allocated using aligned_malloc.
 *
 * Is the same as Buffer.allocUnsafe but is aligned.
 * We need an aligned buffer to create a roaring bitmap frozen view.
 *
 * WARNING: this function is unsafe because the returned buffer may contain previously unallocated memory that may contain sensitive data.
 *
 * @param {number} size The size of the buffer to allocate.
 * @param {number} [alignment=32] The alignment of the buffer to allocate.
 */
export function bufferAlignedAllocUnsafe(size: number, alignment?: number): Buffer;

/**
 * Creates a new buffer backed by a SharedArrayBuffer with the given size and alignment.
 * If alignment is not specified, the default alignment of 32 is used.
 * The buffer does not come from the nodejs buffer pool, it is allocated using aligned_malloc.
 *
 * Is the same as Buffer.alloc but is aligned and uses a SharedArrayBuffer as storage.
 * We need an aligned buffer to create a roaring bitmap frozen view.
 *
 * @param {number} size The size of the buffer to allocate.
 * @param {number} [alignment=32] The alignment of the buffer to allocate.
 */
export function bufferAlignedAllocShared(size: number, alignment?: number): Buffer;

/**
 * Creates a new buffer backed by a SharedArrayBuffer with the given size and alignment.
 * If alignment is not specified, the default alignment of 32 is used.
 * The buffer does not come from the nodejs buffer pool, it is allocated using aligned_malloc.
 *
 * Is the same as Buffer.allocUnsafe but is aligned and uses a SharedArrayBuffer as storage.
 * We need an aligned buffer to create a roaring bitmap frozen view.
 *
 * WARNING: this function is unsafe because the returned buffer may contain previously unallocated memory that may contain sensitive data.
 *
 * @param {number} size The size of the buffer to allocate.
 * @param {number} [alignment=32] The alignment of the buffer to allocate.
 */
export function bufferAlignedAllocSharedUnsafe(size: number, alignment?: number): Buffer;

/**
 * Given some kind of buffer or array buffer, returns a nodejs Buffer instance that contains the same data.
 * The underlying ArrayBuffer is the same.
 *
 * @param buffer The source
 * @returns A nodejs Buffer instance that contains the same data.
 */
export function asBuffer(buffer: Buffer | ArrayBufferView | TypedArray | ArrayBuffer | SharedArrayBuffer): Buffer;

export type TypedArray =
  | Uint8Array
  | Uint16Array
  | Uint32Array
  | Uint8ClampedArray
  | Int8Array
  | Int16Array
  | Int32Array
  | Float32Array
  | Float64Array;

/**
 * Checks if the given buffer is memory aligned.
 * If alignment is not specified, the default alignment of 32 is used.
 *
 * @param {TypedArray | Buffer | ArrayBuffer | SharedArrayBuffer | null | undefined} buffer The buffer to check.
 * @param {number} [alignment=32] The alignment to check.
 */
export function isBufferAligned(
  buffer: TypedArray | Buffer | ArrayBuffer | SharedArrayBuffer | null | undefined,
  alignment?: number,
): boolean;

/**
 * Ensures that the given buffer is aligned to the given alignment.
 * If alignment is not specified, the default alignment of 32 is used.
 * If the buffer is already aligned, it is returned.
 * If the buffer is not aligned, a new aligned buffer is created with bufferAlignedAllocUnsafe or bufferAlignedAllocShared and the data is copied.
 *
 * @param {Buffer} buffer The buffer to align.
 * @param {number} [alignment=32] The alignment to align to.
 * @returns {Buffer} The aligned buffer. Can be the same as the input buffer if it was already aligned. Can be a new buffer if the input buffer was not aligned.
 * @memberof RoaringBitmap32
 */
export function ensureBufferAligned(
  buffer: Buffer | Uint8Array | Uint8ClampedArray | SharedArrayBuffer | Int8Array | ArrayBuffer,
  alignment?: number,
): Buffer;

export enum SerializationFormat {
  /**
   * Stable Optimized non portable C/C++ format. Used by croaring. Can be smaller than the portable format.
   */
  croaring = "croaring",

  /**
   * Stable Portable Java and Go format.
   */
  portable = "portable",

  /**
   * Non portable C/C++ frozen format.
   * Is considered unsafe and unstable because the format might change at any new version.
   * Can be useful for temporary storage or for sending data over the network between similar machines.
   * If the content is corrupted when deserialized or when a frozen view is create, the behavior is undefined!
   * The application may crash, buffer overrun, could be a vector of attack!
   *
   * When this option is used in the serialize function, the new returned buffer (if no buffer was provided) will be aligned to a 32 bytes boundary.
   * This is required to create a frozen view with the method unsafeFrozenView.
   *
   */
  unsafe_frozen_croaring = "unsafe_frozen_croaring",

  /**
   * A plain binary array of 32 bits integers in little endian format. 4 bytes per value.
   */
  uint32_array = "uint32_array",
}

export enum FileSerializationFormat {
  /**
   * Stable Optimized non portable C/C++ format. Used by croaring. Can be smaller than the portable format.
   */
  croaring = "croaring",

  /**
   * Stable Portable Java and Go format.
   */
  portable = "portable",

  /**
   * A plain binary array of 32 bits integers in little endian format. 4 bytes per value.
   */
  uint32_array = "uint32_array",

  /**
   * Non portable C/C++ frozen format.
   * Is considered unsafe and unstable because the format might change at any new version.
   * Can be useful for temporary storage or for sending data over the network between similar machines.
   * If the content is corrupted when deserialized or when a frozen view is create, the behavior is undefined!
   * The application may crash, buffer overrun, could be a vector of attack!
   *
   * When this option is used in the serialize function, the new returned buffer (if no buffer was provided) will be aligned to a 32 bytes boundary.
   * This is required to create a frozen view with the method unsafeFrozenView.
   *
   */
  unsafe_frozen_croaring = "unsafe_frozen_croaring",

  /**
   * Comma separated values, all values are in decimal and in one line without spaces or other characters.
   */
  comma_separated_values = "comma_separated_values",

  /**
   * Tab "\t" separated values, all values are in decimal and in one line without other characters.
   */
  tab_separated_values = "tab_separated_values",

  /**
   * Newline (\n) separated values, all values are in decimal and one per line with a terminating newline.
   */
  newline_separated_values = "newline_separated_values",

  /**
   * A JSON file in the format "[1,2,3,4...]"
   */
  json_array = "json_array",
}

export type SerializationFormatType =
  | SerializationFormat
  | "croaring"
  | "portable"
  | "unsafe_frozen_croaring"
  | "uint32_array"
  | boolean;

export type FileSerializationFormatType =
  | SerializationFormatType
  | FileSerializationFormat
  | "comma_separated_values"
  | "tab_separated_values"
  | "newline_separated_values"
  | "json_array";

export type SerializationDeserializationFormatType = SerializationFormatType & DeserializationFormatType;

export type FileSerializationDeserializationFormatType = FileSerializationFormatType & FileDeserializationFormatType;

export enum DeserializationFormat {
  /** Stable Optimized non portable C/C++ format. Used by croaring. Can be smaller than the portable format. */
  croaring = "croaring",

  /** Stable Portable Java and Go format. */
  portable = "portable",

  /**
   * Non portable C/C++ frozen format.
   * Is considered unsafe and unstable because the format might change at any new version.
   * Can be useful for temporary storage or for sending data over the network between similar machines.
   * If the content is corrupted when loaded or the buffer is modified when a frozen view is create, the behavior is undefined!
   * The application may crash, buffer overrun, could be a vector of attack!
   */
  unsafe_frozen_croaring = "unsafe_frozen_croaring",

  /**
   * Portable version of the frozen view, compatible with Go and Java.
   * Is considered unsafe and unstable because the format might change at any new version.
   * Can be useful for temporary storage or for sending data over the network between similar machines.
   * If the content is corrupted when loaded or the buffer is modified when a frozen view is create, the behavior is undefined!
   * The application may crash, buffer overrun, could be a vector of attack!
   */
  unsafe_frozen_portable = "unsafe_frozen_portable",

  /**
   * A plain binary array of 32 bits integers in little endian format. 4 bytes per value.
   */
  uint32_array = "uint32_array",

  comma_separated_values = "comma_separated_values",
  tab_separated_values = "tab_separated_values",
  newline_separated_values = "newline_separated_values",
  json_array = "json_array",
}

export type DeserializationFormatType =
  | DeserializationFormat
  | "croaring"
  | "portable"
  | "unsafe_frozen_croaring"
  | "unsafe_frozen_portable"
  | "uint32_array"
  | "comma_separated_values"
  | "tab_separated_values"
  | "newline_separated_values"
  | "json_array"
  | boolean;

export enum FileDeserializationFormat {
  /** Stable Optimized non portable C/C++ format. Used by croaring. Can be smaller than the portable format. */
  croaring = "croaring",

  /** Stable Portable Java and Go format. */
  portable = "portable",

  /**
   * Non portable C/C++ frozen format.
   * Is considered unsafe and unstable because the format might change at any new version.
   * Can be useful for temporary storage or for sending data over the network between similar machines.
   * If the content is corrupted when loaded or the buffer is modified when a frozen view is create, the behavior is undefined!
   * The application may crash, buffer overrun, could be a vector of attack!
   */
  unsafe_frozen_croaring = "unsafe_frozen_croaring",

  /**
   * Portable version of the frozen view, compatible with Go and Java.
   * Is considered unsafe and unstable because the format might change at any new version.
   * Can be useful for temporary storage or for sending data over the network between similar machines.
   * If the content is corrupted when loaded or the buffer is modified when a frozen view is create, the behavior is undefined!
   * The application may crash, buffer overrun, could be a vector of attack!
   */
  unsafe_frozen_portable = "unsafe_frozen_portable",

  /**
   * A plain binary array of 32 bits integers in little endian format. 4 bytes per value.
   */
  uint32_array = "uint32_array",

  comma_separated_values = "comma_separated_values",
  tab_separated_values = "tab_separated_values",
  newline_separated_values = "newline_separated_values",
  json_array = "json_array",
}

export type FileDeserializationFormatType = DeserializationFormatType | FileDeserializationFormat;

export enum FrozenViewFormat {
  /**
   * Non portable C/C++ frozen format.
   * Is considered unsafe and unstable because the format might change at any new version.
   * Can be useful for temporary storage or for sending data over the network between similar machines.
   * If the content is corrupted when loaded or the buffer is modified when a frozen view is create, the behavior is undefined!
   * The application may crash, buffer overrun, could be a vector of attack!
   */
  unsafe_frozen_croaring = "unsafe_frozen_croaring",

  /**
   * Portable version of the frozen view, compatible with Go and Java.
   * Is considered unsafe and unstable because the format might change at any new version.
   * Can be useful for temporary storage or for sending data over the network between similar machines.
   * If the content is corrupted when loaded or the buffer is modified when a frozen view is create, the behavior is undefined!
   * The application may crash, buffer overrun, could be a vector of attack!
   */
  unsafe_frozen_portable = "unsafe_frozen_portable",
}

export type FrozenViewFormatType = FrozenViewFormat | "unsafe_frozen_croaring" | "unsafe_frozen_portable";

export interface ReadonlyRoaringBitmap32 extends ReadonlySet<number> {
  /**
   * Property. Gets the number of items in the set (cardinality).
   *
   * @type {number}
   * @memberof ReadonlyRoaringBitmap32
   */
  get size(): number;

  /**
   * Property. True if the bitmap is empty.
   *
   * @type {boolean}
   * @memberof ReadonlyRoaringBitmap32
   */
  get isEmpty(): boolean;

  /**
   * Property. True if the bitmap is read-only.
   * A read-only bitmap cannot be modified, every operation will throw an error.
   * You can freeze a bitmap using the freeze() method.
   * A bitmap cannot be unfrozen.
   *
   * @type {boolean}
   * @memberof ReadonlyRoaringBitmap32
   */
  get isFrozen(): boolean;

  /**
   * [Symbol.iterator]() Gets a new iterator able to iterate all values in the set in ascending order.
   *
   * WARNING: Is not allowed to change the bitmap while iterating.
   * The iterator may throw exception if the bitmap is changed during the iteration.
   *
   * @returns {RoaringBitmap32Iterator} A new iterator
   * @memberof ReadonlyRoaringBitmap32
   */
  [Symbol.iterator](): RoaringBitmap32Iterator;

  /**
   * Gets a new iterator able to iterate all values in the set in ascending order.
   *
   * WARNING: Is not allowed to change the bitmap while iterating.
   * The iterator may throw exception if the bitmap is changed during the iteration.
   *
   * Same as [Symbol.iterator]()
   *
   * @returns {RoaringBitmap32Iterator} A new iterator
   * @memberof ReadonlyRoaringBitmap32
   */
  iterator(): RoaringBitmap32Iterator;

  /**
   * Gets a new iterator able to iterate all values in the set in descending order.
   *
   * WARNING: Is not allowed to change the bitmap while iterating.
   * The iterator may throw exception if the bitmap is changed during the iteration.
   *
   * @returns {RoaringBitmap32Iterator} A new reverse iterator
   * @memberof ReadonlyRoaringBitmap32
   */
  reverseIterator(): RoaringBitmap32Iterator;

  /**
   * Gets a new iterator able to iterate all values in the set in ascending order.
   * This is just for compatibility with the Set<number> interface.
   *
   * WARNING: Is not allowed to change the bitmap while iterating. Undefined behaviour.
   * The iterator may throw exception if the bitmap is changed during the iteration.
   *
   * Same as [Symbol.iterator]()
   *
   * @returns {RoaringBitmap32Iterator} A new iterator
   * @memberof ReadonlyRoaringBitmap32
   */
  keys(): RoaringBitmap32Iterator;

  /**
   * Gets a new iterator able to iterate all values in the set in ascending order.
   * This is just for compatibility with the Set<number> interface.
   *
   * WARNING: Is not allowed to change the bitmap while iterating. Undefined behaviour.
   * The iterator may throw exception if the bitmap is changed during the iteration.
   *
   * Same as [Symbol.iterator]()
   *
   * @returns {RoaringBitmap32Iterator} A new iterator
   * @memberof ReadonlyRoaringBitmap32
   */
  values(): RoaringBitmap32Iterator;

  /**
   * Gets a new iterator able to iterate all value pairs [value, value] in the set in ascending order.
   * This is just for compatibility with the Set<number> interface.
   *
   * WARNING: Is not allowed to change the bitmap while iterating. Undefined behaviour.
   * The iterator may throw exception if the bitmap is changed during the iteration.
   *
   * Same as [Symbol.iterator]()
   *
   * @returns {RoaringBitmap32Iterator} A new iterator
   * @memberof ReadonlyRoaringBitmap32
   */
  entries(): IterableIterator<[number, number]>;

  /**
   * Executes a function for each value in the set, in ascending order.
   * The callback has 3 arguments, the value, the value and this (this set). This is to match the Set<number> interface.
   *
   * WARNING: Is not allowed to change the bitmap while iterating. Undefined behaviour.
   *
   * WARNING: the second parameter of the callback is not the index, but the value itself, the same as the first argument.
   * This is required to match the Set<number> interface.
   */
  forEach<This = unknown>(
    callbackfn: (this: This, value: number, index: number, set: this) => void,
    thisArg?: This,
  ): this;

  /**
   * Behaves like array.some.
   * The some() method tests whether at least one element in the set passes the test implemented by the provided function.
   * It returns true if, in the set, it finds an element for which the provided function returns true; otherwise it returns false.
   *
   * WARNING: this can potentially iterate a large set of to 4 billion elements.
   *
   * WARNING: Is not allowed to change the bitmap while iterating. Undefined behaviour.
   *
   * @see https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/some
   */
  some(callbackfn: (value: number, index: number, set: this) => boolean, thisArg?: unknown): boolean;

  /**
   * Behaves like array.every.
   * The every() method tests whether all elements in the set pass the test implemented by the provided function.
   * It returns a Boolean value.
   *
   * WARNING: this can potentially iterate a large set of to 4 billion elements.
   *
   * WARNING: Is not allowed to change the bitmap while iterating. Undefined behaviour.
   *
   * @see https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/every
   */
  every(callbackfn: (value: number, index: number, set: this) => boolean, thisArg?: unknown): boolean;

  /**
   * Behaves like array.reduce.
   * The reduce() method applies a function against an accumulator and each value of the set (from left-to-right) to reduce it to a single value.
   *
   * WARNING: this can potentially iterate a large set of to 4 billion elements.
   *
   * WARNING: Is not allowed to change the bitmap while iterating. Undefined behaviour.
   *
   * @param callbackfn A function that accepts up to four arguments. The reduce method calls the callbackfn function one time for each element in the set.
   * @returns The value that results from the reduction.
   * @see https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/reduce
   * @memberof ReadonlyRoaringBitmap32
   */
  reduce(callbackfn: (previousValue: number, currentValue: number, currentIndex: number, set: this) => number): number;
  reduce(
    callbackfn: (previousValue: number, currentValue: number, currentIndex: number, set: this) => number,
    initialValue: number | undefined,
  ): number;
  reduce<U>(
    callbackfn: (previousValue: U, currentValue: number, currentIndex: number, set: this) => U,
    initialValue: U,
  ): U;

  /**
   * Behaves like array.reduceRight.
   * The reduceRight() method applies a function against an accumulator and each value of the set (from right-to-left) to reduce it to a single value.
   * WARNING: this can potentially iterate a large set of to 4 billion elements.
   * WARNING: Is not allowed to change the bitmap while iterating. Undefined behaviour.
   * @param callbackfn A function that accepts up to four arguments. The reduce method calls the callbackfn function one time for each element in the set.
   * @returns The value that results from the reduction.
   * @see https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/reduceRight
   * @memberof ReadonlyRoaringBitmap32
   */
  reduceRight(
    callbackfn: (previousValue: number, currentValue: number, currentIndex: number, set: this) => number,
  ): number;
  reduceRight(
    callbackfn: (previousValue: number, currentValue: number, currentIndex: number, set: this) => number,
    initialValue: number | undefined,
  ): number;
  reduceRight<U>(
    callbackfn: (previousValue: U, currentValue: number, currentIndex: number, set: this) => U,
    initialValue: U,
  ): U;

  /**
   * Behaves like array.find.
   * The find() method returns the value of the first element in the set that satisfies the provided testing function.
   * Otherwise undefined is returned.
   * WARNING: this can potentially iterate a large set of to 4 billion elements.
   * WARNING: Is not allowed to change the bitmap while iterating. Undefined behaviour.
   * @param predicate find calls predicate once for each element of the set, in ascending order, until it finds one where predicate returns true.
   * If such an element is found, find immediately returns that element value. Otherwise, find returns undefined.
   * @param thisArg If provided, it will be used as the this value for each invocation of predicate.
   * If it is not provided, undefined is used instead.
   * @returns The value of the first element in the set that satisfies the provided testing function.
   * Otherwise undefined is returned.
   * @see https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/find
   * @memberof ReadonlyRoaringBitmap32
   */
  find(predicate: (value: number, index: number, set: this) => boolean, thisArg?: unknown): number | undefined;

  /**
   * Behaves like array.findIndex.
   * The findIndex() method returns the index of the first element in the set that satisfies the provided testing function.
   * Otherwise, it returns -1, indicating that no element passed the test.
   * WARNING: this can potentially iterate a large set of to 4 billion elements.
   * WARNING: Is not allowed to change the bitmap while iterating. Undefined behaviour.
   * @param predicate find calls predicate once for each element of the set, in ascending order, until it finds one where predicate returns true.
   * If such an element is found, findIndex immediately returns that element index. Otherwise, findIndex returns -1.
   * @param thisArg If provided, it will be used as the this value for each invocation of predicate.
   * If it is not provided, undefined is used instead.
   * @returns The index of the first element in the set that satisfies the provided testing function.
   * Otherwise, it returns -1, indicating that no element passed the test.
   * @see https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/findIndex
   * @memberof ReadonlyRoaringBitmap32
   */
  findIndex(predicate: (value: number, index: number, set: this) => boolean, thisArg?: unknown): number;

  /**
   * It behaves like array.map.
   * WARNING: The returned array may be very big, up to 4 billion elements.
   * WARNING: Is not allowed to change the bitmap while iterating. Undefined behaviour.
   *
   * @param callbackfn A function that accepts up to three arguments. The map method calls the callbackfn function one time for each element in the array.
   * @param thisArg An object to which the this keyword can refer in the callbackfn function. If thisArg is omitted, undefined is used as the this value.
   * @returns An array containing the results of calling the callbackfn function on each element in the set.
   * @memberof ReadonlyRoaringBitmap32
   * @see https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/map
   */
  map<U, This = unknown>(
    callbackfn: (this: This, value: number, index: number, set: this) => U,
    thisArg?: This,
    output?: U[],
  ): U[];

  /**
   * It behaves like array.filter.
   * WARNING: The returned array may be very big, up to 4 billion elements.
   * WARNING: Is not allowed to change the bitmap while iterating. Undefined behaviour.
   *
   * @param callbackfn A function that accepts up to three arguments. The filter method calls the callbackfn function one time for each element in the array.
   * @param thisArg An object to which the this keyword can refer in the callbackfn function. If thisArg is omitted, undefined is used as the this value.
   * @returns A new array containing all elements of the array that satisfy the given predicate.
   * @memberof ReadonlyRoaringBitmap32
   * @see https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/filter
   */
  filter(
    predicate: (value: number, index: number, set: this) => boolean,
    thisArg?: unknown,
    output?: number[],
  ): number[];

  /**
   * It behaves like array.toSorted.
   * Returns a new array that is this set sorted according to the compare function.
   * If no sorting function is provided, the array is sorted according to the numeric order of the values (the same as calling this.toArray()).
   *
   * WARNING: The returned array may be very big, up to 4 billion elements.
   * WARNING: Is not allowed to change the bitmap while iterating. Undefined behaviour.
   *
   * @param cmp A function that defines an alternative sort order. The sort method calls the compareFunction function
   * once for each element in the array.
   * @returns A new sorted array that contains all the elements of this set.
   * @memberof ReadonlyRoaringBitmap32
   * @see https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/toSorted
   */
  toSorted(cmp?: (a: number, b: number) => number): number[];

  /**
   * It behaves like array.toReversed.
   * Returns a new array that is this set sorted in reverse order.
   * WARNING: The returned array may be very big, up to 4 billion elements.
   * @returns An array containing the elements of this set in reverse order (descending).
   */
  toReversed(): number[];

  /**
   * Gets the minimum value in the set.
   *
   * @returns {number} The minimum value in the set or 0xFFFFFFFF if empty.
   * @memberof ReadonlyRoaringBitmap32
   */
  minimum(): number;

  /**
   * Gets the maximum value in the set.
   *
   * @returns {number} The minimum value in the set or 0 if empty.
   * @memberof ReadonlyRoaringBitmap32
   */
  maximum(): number;

  /**
   * Checks wether the given value exists in the set.
   * Is the same as this.includes(value).
   *
   * @param {number} value A 32 bit unsigned integer to search.
   * @returns {boolean} True if the set contains the given value, false if not.
   * @memberof ReadonlyRoaringBitmap32
   */
  has(value: unknown): boolean;

  /**
   * Checks wether the given value exists in the set.
   * Is the same as this.has(value).
   *
   * @param {number} value A 32 bit unsigned integer to search.
   * @returns {boolean} True if the set contains the given value, false if not.
   * @memberof ReadonlyRoaringBitmap32
   */
  includes(value: unknown): boolean;

  /**
   * Returns the index of value in the set, index start from 0.
   * If the set doesn't contain value, this function will return -1.
   * The difference with rank function is that this function will return -1 when value isn't in the set, but the rank function will return a non-negative number.
   *
   * @param {number} value A 32 bit unsigned integer to search.
   * @param {number} [fromIndex] The index to start the search at, defaults to 0. It does not have performance difference, is just for compatibility with array.indexOf.
   * @returns {boolean} True if the set contains the given value, false if not.
   * @memberof ReadonlyRoaringBitmap32
   */
  indexOf(value: unknown, fromIndex?: number): number;

  /**
   * Returns the index of value in the set, index start from 0.
   * If the set doesn't contain value, this function will return -1.
   * The difference with rank function is that this function will return -1 when value isn't in the set, but the rank function will return a non-negative number.
   * If fromIndex is not specified, is the same as this.indexOf(value).
   * It behaves like array.lastIndexOf, but it doesn't have performance difference, is just for compatibility with array.lastIndexOf.
   *
   * @param {number} value A 32 bit unsigned integer to search.
   * @param {number} [fromIndex] The index to start the search at, defaults to 0. It does not have performance difference, is just for compatibility with array.indexOf.
   * @returns {boolean} True if the set contains the given value, false if not.
   * @memberof ReadonlyRoaringBitmap32
   */
  lastIndexOf(value: unknown, fromIndex?: number): number;

  /**
   * Check whether a range of values from rangeStart (included) to rangeEnd (excluded) is present
   *
   * @param {number|undefined} rangeStart The start index (inclusive).
   * @param {number|undefined} [rangeEnd] The end index (exclusive).
   * @returns {boolean} True if the bitmap contains the whole range of values from rangeStart (included) to rangeEnd (excluded), false if not.
   * @memberof ReadonlyRoaringBitmap32
   */
  hasRange(rangeStart: number | undefined, rangeEnd?: number | undefined): boolean;

  /**
   * Gets the cardinality (number of elements) between rangeStart (included) to rangeEnd (excluded) of the bitmap.
   * Returns 0 if range is invalid or if no element was found in the given range.
   *
   * @param {number|undefined} rangeStart The start index (inclusive).
   * @param {number|undefined} [rangeEnd] The end index (exclusive).
   * @returns {number} The number of elements between rangeStart (included) to rangeEnd (excluded).
   */
  rangeCardinality(rangeStart: number | undefined, rangeEnd?: number | undefined): number;

  /**
   * Checks wether this set is a subset or the same as the given set.
   *
   * Returns false also if the given argument is not a ReadonlyRoaringBitmap32 instance.
   *
   * @param {ReadonlyRoaringBitmap32} other The other set.
   * @returns {boolean} True if this set is a subset of the given ReadonlyRoaringBitmap32. False if not.
   * @memberof ReadonlyRoaringBitmap32
   */
  isSubset(other: ReadonlyRoaringBitmap32): boolean;

  /**
   * Checks wether this set is a strict subset of the given set.
   *
   * Returns false if the sets are the same.
   *
   * Returns false also if the given argument is not a ReadonlyRoaringBitmap32 instance.
   *
   * @param {ReadonlyRoaringBitmap32} other The other ReadonlyRoaringBitmap32 instance.
   * @returns {boolean} True if this set is a strict subset of the given ReadonlyRoaringBitmap32. False if not.
   * @memberof ReadonlyRoaringBitmap32
   */
  isStrictSubset(other: ReadonlyRoaringBitmap32): boolean;

  /**
   * Checks wether this set is equal to another set.
   *
   * Returns false also if the given argument is not a ReadonlyRoaringBitmap32 instance.
   *
   * @param {ReadonlyRoaringBitmap32} other The other set to compare for equality.
   * @returns {boolean} True if the two sets contains the same elements, false if not.
   * @memberof ReadonlyRoaringBitmap32
   */
  isEqual(other: ReadonlyRoaringBitmap32): boolean;

  /**
   * Check whether the two bitmaps intersect.
   *
   * Returns true if there is at least one item in common, false if not.
   *
   * Returns false also if the given argument is not a ReadonlyRoaringBitmap32 instance.
   *
   * @param {ReadonlyRoaringBitmap32} other The other set to compare for intersection.
   * @returns {boolean}  True if the two set intersects, false if not.
   * @memberof ReadonlyRoaringBitmap32
   */
  intersects(other: ReadonlyRoaringBitmap32): boolean;

  /**
   * Check whether a bitmap and a closed range intersect.
   *
   * @param {number|undefined} rangeStart The start of the range.
   * @param {number|undefined} [rangeEnd] The end of the range.
   * @returns {boolean} True if the bitmap and the range intersects, false if not.
   */
  intersectsWithRange(rangeStart: number | undefined, rangeEnd?: number | undefined): boolean;

  /**
   * Computes the size of the intersection between two bitmaps (the number of values in common).
   *
   * Returns -1 if the given argument is not a ReadonlyRoaringBitmap32 instance.
   *
   * @param {ReadonlyRoaringBitmap32} other The other set to compare for intersection.
   * @returns {number} The number of elements in common.
   * @memberof ReadonlyRoaringBitmap32
   */
  andCardinality(other: ReadonlyRoaringBitmap32): number;

  /**
   * Computes the size of the union between two bitmaps.
   *
   * Returns -1 if the given argument is not a ReadonlyRoaringBitmap32 instance.
   *
   * @param {ReadonlyRoaringBitmap32} other The other set to compare for intersection.
   * @returns {number} The number of elements in common.
   * @memberof ReadonlyRoaringBitmap32
   */
  orCardinality(other: ReadonlyRoaringBitmap32): number;

  /**
   * Computes the size of the difference (andnot) between two bitmaps.
   *
   * Returns -1 if the given argument is not a ReadonlyRoaringBitmap32 instance.
   *
   * @param {ReadonlyRoaringBitmap32} other The other set to compare for intersection.
   * @returns {number}  The number of elements in common.
   * @memberof ReadonlyRoaringBitmap32
   */
  andNotCardinality(other: ReadonlyRoaringBitmap32): number;

  /**
   * Computes the size of the symmetric difference (xor) between two bitmaps.
   *
   * Returns -1 if the given argument is not a ReadonlyRoaringBitmap32 instance.
   *
   * @param {ReadonlyRoaringBitmap32} other The other set to compare for intersection.
   * @returns {number} The number of elements in common.
   * @memberof ReadonlyRoaringBitmap32
   */
  xorCardinality(other: ReadonlyRoaringBitmap32): number;

  /**
   * Computes the Jaccard index between two bitmaps.
   * (Also known as the Tanimoto distance or the Jaccard similarity coefficient).
   *
   * See https://en.wikipedia.org/wiki/Jaccard_index
   *
   * The Jaccard index is undefined if both bitmaps are empty.
   *
   * Returns -1 if the given argument is not a ReadonlyRoaringBitmap32 instance.
   *
   * @param {ReadonlyRoaringBitmap32} other The other ReadonlyRoaringBitmap32 to compare.
   * @returns {number} The Jaccard index.
   * @memberof ReadonlyRoaringBitmap32
   */
  jaccardIndex(other: ReadonlyRoaringBitmap32): number;

  /**
   *  Returns the number of values in the set that are smaller or equal to the given value.
   *
   * @param {number} maxValue The maximum value
   * @returns {number} Returns the number of values in the set that are smaller or equal to the given value.
   * @memberof ReadonlyRoaringBitmap32
   */
  rank(maxValue: number): number;

  /**
   * If the size of the roaring bitmap is strictly greater than rank,
   * then this function returns the element of given rank.
   *
   * Otherwise, it returns undefined.
   *
   * @param {number} rank The rank, an unsigned 32 bit integer.
   * @returns {(number | undefined)} The element of the given rank or undefined if not found.
   * @memberof ReadonlyRoaringBitmap32
   */
  select(rank: number): number | undefined;

  /**
   * Creates a new Uint32Array and fills it with all the values in the bitmap.
   *
   * The returned array may be very big, up to 4 billion elements.
   *
   * Use this function only when you know what you are doing.
   *
   * This function is faster than calling new Uint32Array(bitmap);
   *
   * See rangeUint32Array to paginate.
   *
   * @returns A new Uint32Array instance containing all the items in the set in order.
   * @memberof ReadonlyRoaringBitmap32
   */
  toUint32Array(): Uint32Array;

  /**
   * Creates a new Uint32Array and fills it with all the values in the bitmap up to the given length.
   *
   * See rangeUint32Array to paginate.
   *
   * @returns A new Uint32Array instance containing all the items in the set in order.
   * @memberof ReadonlyRoaringBitmap32
   */
  toUint32Array(maxSize: number): Uint32Array;

  /**
   * Copies all the values in the roaring bitmap to an Uint32Array.
   *
   * This function is faster than calling new Uint32Array(bitmap);
   * Throws if the given array is not a valid Uint32Array or Int32Array or is not big enough.
   *
   * See rangeUint32Array to paginate.
   *
   * @param {TOutput} output The output array.
   * @returns {Uint32Array} The output array. Limited to the resulting size.
   * @memberof ReadonlyRoaringBitmap32
   */
  toUint32Array(output: Uint32Array | Int32Array | ArrayBuffer | SharedArrayBuffer): Uint32Array;

  /**
   * Creates a new Uint32Array and fills it with all the values in the bitmap, asynchronously.
   * The bitmap will be temporarily frozen until the operation completes.
   *
   * The returned array may be very big, up to 4 billion elements.
   *
   * Use this function only when you know what you are doing.
   *
   * This function is faster than calling new Uint32Array(bitmap);
   *
   * See rangeUint32Array to paginate.
   *
   * @returns A new Uint32Array instance containing all the items in the set in order.
   * @memberof ReadonlyRoaringBitmap32
   */
  toUint32ArrayAsync(): Promise<Uint32Array>;

  /**
   * Copies all the values in the roaring bitmap to an Uint32Array, asynchronously.
   * The bitmap will be temporarily frozen until the operation completes.
   *
   * This function is faster than calling new Uint32Array(bitmap);
   * Throws if the given array is not a valid Uint32Array or Int32Array or is not big enough.
   *
   * See rangeUint32Array to paginate.
   *
   * @param {TOutput} output The output array.
   * @returns {Uint32Array} The output array. Limited to the resulting size.
   * @memberof ReadonlyRoaringBitmap32
   */
  toUint32ArrayAsync(output: Uint32Array | Int32Array | ArrayBuffer | SharedArrayBuffer): Promise<Uint32Array>;

  /**
   * toUint32Array array with pagination
   * @returns A new Uint32Array instance containing paginated items in the set in order.
   * @memberof ReadonlyRoaringBitmap32
   */
  rangeUint32Array(
    output: Uint32Array | Int32Array | ArrayBuffer | SharedArrayBuffer,
    offset: number,
    limit?: number | undefined,
  ): Uint32Array;

  /**
   * toUint32Array array with pagination
   * @returns A new Uint32Array instance containing paginated items in the set in order.
   * @memberof ReadonlyRoaringBitmap32
   */
  rangeUint32Array(minimumValue: number, maximumValue: number): Uint32Array;

  /**
   * toUint32Array array with pagination
   * @param {TOutput} output The output array.
   * @returns {Uint32Array} The output array. Limited to the resulting size.
   * @memberof ReadonlyRoaringBitmap32
   */
  rangeUint32Array(
    minimumValue: number,
    maximumValue: number,
    output: Uint32Array | Int32Array | ArrayBuffer | SharedArrayBuffer,
  ): Uint32Array;

  /**
   * toUint32Array array with pagination
   * @param {TOutput} output The output array.
   * @returns {Uint32Array} The output array. Limited to the resulting size.
   * @memberof ReadonlyRoaringBitmap32
   */
  rangeUint32Array(
    minimumValue: number,
    output: Uint32Array | Int32Array | ArrayBuffer | SharedArrayBuffer,
  ): Uint32Array;

  /**
   * Same as toUint32Array
   * @param {TOutput} output The output array.
   * @returns {Uint32Array} The output array. Limited to the resulting size.
   * @memberof ReadonlyRoaringBitmap32
   */
  rangeUint32Array(output: Uint32Array | Int32Array | ArrayBuffer | SharedArrayBuffer): Uint32Array;

  /**
   * Creates a new plain JS array and fills it with all the values in the bitmap.
   *
   * The returned array may be very big, use this function only when you know what you are doing.
   *
   * @param {number} [maxLength] The maximum number of elements to return.
   * @returns {number[]}  A new plain JS array that contains all the items in the set in order.
   * @memberof ReadonlyRoaringBitmap32
   */
  toArray(maxLength?: number | undefined): number[];

  /**
   * Append all the values in this bitmap to the given plain JS array.
   *
   * The resulting array may be very big, use this function only when you know what you are doing.
   *
   * @template TOutput The type of the output array.
   * @param {TOutput} output The output array.
   * @param {number} [maxLength] The maximum number of elements to return.
   * @returns {TOutput} The output array.
   * @memberof ReadonlyRoaringBitmap32
   */
  toArray<TOutput extends number[]>(output: TOutput, maxLength?: number, offset?: number): TOutput;

  /**
   * Creates a new plain JS Set<number> and fills it with all the values in the bitmap.
   *
   * The returned set may be very big, use this function only when you know what you are doing.
   *
   * @param {number} [maxLength] The maximum number of elements to return.
   * @returns {Set<number>} A new plain JS array that contains all the items in the set in order.
   * @memberof ReadonlyRoaringBitmap32
   */
  toSet(maxLength?: number | undefined): Set<number>;

  /**
   * Adds all the values in this bitmap to the given plain JS Set<number>.
   *
   * The resulting set may be very big, use this function only when you know what you are doing.
   *
   * @param {Set<number>} output The output set.
   * @param {number} [maxLength] The maximum number of elements to return.
   * @returns {Set<number>} The output set.
   * @memberof ReadonlyRoaringBitmap32
   */
  toSet(output: Set<number>, maxLength?: number | undefined): Set<number>;

  /**
   * Returns a plain JS array with all the values in the bitmap.
   *
   * Used by JSON.stringify to serialize this bitmap as an array.
   *
   * @returns {number[]} A new plain JS array that contains all the items in the set in order.
   * @memberof ReadonlyRoaringBitmap32
   */
  toJSON(): number[];

  /**
   * How many bytes are required to serialize this bitmap.
   *
   * Setting the format flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
   * The portable version is meant to be compatible with Java and Go versions.
   *
   * @param {SerializationFormat | boolean} format One of the SerializationFormat enum values, or a boolean value: if false, optimized C/C++ format is used. If true, Java and Go portable format is used.
   * @returns {number} How many bytes are required to serialize this bitmap.
   * @memberof ReadonlyRoaringBitmap32
   */
  getSerializationSizeInBytes(format: SerializationFormatType): number;

  /**
   * Serializes the bitmap into a new Buffer.
   *
   * Setting the formatg to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
   * The portable version is meant to be compatible with Java and Go versions.
   *
   * @param {SerializationFormat | boolean} format One of the SerializationFormat enum values, or a boolean value: if false, optimized C/C++ format is used. If true, Java and Go portable format is used.
   * @returns {Buffer} A new node Buffer that contains the serialized bitmap.
   * @memberof ReadonlyRoaringBitmap32
   */
  serialize(format: SerializationFormatType, _output?: undefined): Buffer;

  /**
   * Serializes the bitmap into the given Buffer, starting to write at the given outputStartIndex position.
   * The operation will fail with an error if the buffer is smaller than what getSerializationSizeInBytes(format) returns.
   *
   * Setting the format to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
   * The portable version is meant to be compatible with Java and Go versions.
   *
   * @param {boolean} format If false, optimized C/C++ format is used. If true, Java and Go portable format is used.
   * @param {Buffer} output The node Buffer where to write the serialized data.
   * @returns {Buffer} The output Buffer. If the input buffer was exactly of the same size. Otherwise, a new buffer backed by the same storage is returned, with the correct offset and length.
   * @memberof ReadonlyRoaringBitmap32
   */
  serialize(
    format: SerializationFormatType,
    output: Uint8Array | Int8Array | Uint8ClampedArray | ArrayBuffer | SharedArrayBuffer,
  ): Buffer;

  /**
   * Serializes the bitmap into the given Buffer, starting to write at position 0.
   * The operation will fail with an error if the buffer is smaller than what getSerializationSizeInBytes(format) returns.
   *
   * Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
   * The portable version is meant to be compatible with Java and Go versions.
   *
   * @param {boolean} portable If false, optimized C/C++ format is used. If true, Java and Go portable format is used.
   * @param {Buffer} output The node Buffer where to write the serialized data.
   * @returns {Buffer} The output Buffer. If the input buffer was exactly of the same size. Otherwise, a new buffer backed by the same storage is returned, with the correct offset and length.
   * @memberof ReadonlyRoaringBitmap32
   */
  serialize(
    output: Uint8Array | Int8Array | Uint8ClampedArray | ArrayBuffer | SharedArrayBuffer,
    format: SerializationFormatType,
  ): Buffer;

  /**
   * Serializes the bitmap into a new Buffer.
   * The bitmap will be temporarily frozen until the operation completes.
   *
   * Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
   * The portable version is meant to be compatible with Java and Go versions.
   *
   * @param {SerializationFormat | boolean} format One of the SerializationFormat enum values, or a boolean value: if false, optimized C/C++ format is used. If true, Java and Go portable format is used.
   * @returns {Buffer} A new node Buffer that contains the serialized bitmap.
   * @memberof ReadonlyRoaringBitmap32
   */
  serializeAsync(format: SerializationFormatType, _output?: undefined): Promise<Buffer>;

  /**
   * Serializes the bitmap into the given Buffer, starting to write at the given outputStartIndex position.
   * The bitmap will be temporarily frozen until the operation completes.
   * The operation will fail with an error if the buffer is smaller than what getSerializationSizeInBytes(format) returns.
   *
   * Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
   * The portable version is meant to be compatible with Java and Go versions.
   *
   * @param {boolean} format If false, optimized C/C++ format is used. If true, Java and Go portable format is used.
   * @param {Buffer} output The node Buffer where to write the serialized data.
   * @returns {Promise<Buffer>} The output Buffer. If the input buffer was exactly of the same size, the same buffer is returned. Otherwise, a new buffer backed by the same storage is returned, with the correct offset and length.
   * @memberof ReadonlyRoaringBitmap32
   */
  serializeAsync(
    format: SerializationFormatType,
    output: Uint8Array | Int8Array | Uint8ClampedArray | ArrayBuffer | SharedArrayBuffer,
  ): Promise<Buffer>;

  /**
   * Serializes the bitmap into the given Buffer, starting to write at position 0.
   * The bitmap will be temporarily frozen until the operation completes.
   * The operation will fail with an error if the buffer is smaller than what getSerializationSizeInBytes(format) returns.
   *
   * Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
   * The portable version is meant to be compatible with Java and Go versions.
   *
   * @param {boolean} portable If false, optimized C/C++ format is used. If true, Java and Go portable format is used.
   * @param {Buffer} output The node Buffer where to write the serialized data.
   * @returns {Promise<Buffer>} The output Buffer. If the input buffer was exactly of the same size, the same buffer is returned. Otherwise, a new buffer backed by the same storage is returned, with the correct offset and length.
   * @memberof ReadonlyRoaringBitmap32
   */
  serializeAsync(
    output: Uint8Array | Int8Array | Uint8ClampedArray | ArrayBuffer | SharedArrayBuffer,
    format: SerializationFormatType,
  ): Promise<Buffer>;

  /**
   * Serializes the bitmap into a file, asynchronously.
   * The bitmap will be temporarily frozen until the operation completes.
   *
   * This is faster, everything runs in its own thread and it consumes less memory than serializing to a Buffer and then to write to a file,
   * internally it uses memory mapped files and skip all the JS overhead.
   *
   * @param {FileSerializationFormat | boolean} format One of the SerializationFormat enum values, or a boolean value: if false, optimized C/C++ format is used. If true, Java and Go portable format is used.
   * @memberof ReadonlyRoaringBitmap32
   */
  serializeFileAsync(filePath: string, format: FileSerializationFormatType): Promise<void>;

  /**
   * Returns a new bitmap that is a copy of this bitmap, same as new RoaringBitmap32(copy)
   *
   * @returns {RoaringBitmap32} A cloned RoaringBitmap32 instance
   * @memberof ReadonlyRoaringBitmap32
   */
  clone(): RoaringBitmap32;

  /**
   * Returns always "ReadonlyRoaringBitmap32".
   *
   * To have a standard string representation of the content as a string, call contentToString() instead.
   *
   * @returns {string} "Set"
   * @memberof ReadonlyRoaringBitmap32
   */
  [Symbol.toStringTag]: "Set";

  /**
   * Returns always "ReadonlyRoaringBitmap32".
   *
   * To have a standard string representation of the content as a string, call contentToString() instead.
   *
   * @returns {string} "ReadonlyRoaringBitmap32"
   * @memberof ReadonlyRoaringBitmap32
   */
  toString(): string;

  /**
   * Returns a standard string representation of the content of this ReadonlyRoaringBitmap32 instance. It may return a very long string.
   *
   * Default max length is 32000 characters, everything after maxLength is truncated (ellipsis added).
   *
   * @param {number} [maxLength] Approximate maximum length of the string. Default is 32000. Ellipsis will be added if the string is longer.
   * @returns {string} A string in the format "[1,2,3...]"
   * @memberof ReadonlyRoaringBitmap32
   */
  contentToString(maxLength?: number): string;

  /**
   * Returns a standard string representation of the content of this ReadonlyRoaringBitmap32 instance.
   *
   * WARNING: this can potentially iterate a large set of to 4 billion elements.
   *
   * @param {string} [separator] The separator to use between elements. Default is ",".
   * @returns {string} A string in the format "1<separator>2<separator>3..."
   * @memberof ReadonlyRoaringBitmap32
   */
  join(separator?: string): string;

  /**
   * Returns an object that contains statistic information about this ReadonlyRoaringBitmap32 instance.
   *
   * @returns {RoaringBitmap32Statistics} An object containing several statistics for the bitmap.
   * @memberof ReadonlyRoaringBitmap32
   */
  statistics(): RoaringBitmap32Statistics;
}

export interface RoaringBitmap32 extends ReadonlyRoaringBitmap32, Set<number> {
  /**
   * Property: The version of the CRoaring libary as a string.
   * Example: "0.9.2"
   *
   * @export
   * @constant
   * @type {string} The version of the CRoaring libary as a string. Example: "0.9.2"
   * @memberof RoaringBitmap32
   */
  get CRoaringVersion(): string;

  /**
   * Property: The version of the roaring npm package as a string.
   * Example: "1.2.0"
   *
   * @export
   * @constant
   * @type {string} The version of the roaring npm package as a string. Example: "0.4.1"
   * @memberof RoaringBitmap32
   */
  get PackageVersion(): string;

  /**
   * [Symbol.iterator]() Gets a new iterator able to iterate all values in the set in ascending order.
   *
   * WARNING: Is not allowed to change the bitmap while iterating.
   * The iterator may throw exception if the bitmap is changed during the iteration.
   *
   * @returns {RoaringBitmap32Iterator} A new iterator
   * @memberof RoaringBitmap32
   */
  [Symbol.iterator](): RoaringBitmap32Iterator;

  /**
   * Returns always "RoaringBitmap32".
   *
   * To have a standard string representation of the content as a string, call contentToString() instead.
   *
   * @returns {string} "Set"
   * @memberof RoaringBitmap32
   */
  [Symbol.toStringTag]: "Set";

  /**
   * The at() method takes an integer value and returns the item at that index,
   * allowing for positive and negative integers. Negative integers count back from the last item in the set.
   *
   * @param {number} index Zero-based index of the array element to be returned, converted to an integer. Negative index counts back from the end of the array — if index < 0, index + array.length is accessed.
   * @returns {number | undefined} The element in the set matching the given index. Always returns undefined if index < -array.length or index >= array.length without attempting to access the corresponding property.
   * @memberof RoaringBitmap32
   */
  at(index: number): number | undefined;

  /**
   * Removes all values from the set.
   *
   * It frees resources, you can use clear() to free some memory before the garbage collector disposes this instance.
   *
   * @memberof RoaringBitmap32
   */
  clear(): boolean;

  /**
   * Executes a function for each value in the set, in ascending order.
   * The callback has 3 arguments, the value, the value and this (this set). This is to match the Set<number> interface.
   *
   * WARNING: Is not allowed to change the bitmap while iterating. Undefined behaviour.
   *
   * WARNING: the second parameter of the callback is not the index, but the value itself, the same as the first argument.
   * This is required to match the Set<number> interface.
   */
  forEach<This = unknown>(
    callbackfn: (this: This, value: number, value2: number, set: this) => void,
    thisArg?: This,
  ): this;

  /**
   * Gets a new iterator able to iterate all values in the set in ascending order.
   * This is just for compatibility with the Set<number> interface.
   *
   * WARNING: Is not allowed to change the bitmap while iterating. Undefined behaviour.
   * The iterator may throw exception if the bitmap is changed during the iteration.
   *
   * Same as [Symbol.iterator]()
   *
   * @returns {RoaringBitmap32Iterator} A new iterator
   * @memberof RoaringBitmap32
   */
  keys(): RoaringBitmap32Iterator;

  /**
   * Gets a new iterator able to iterate all values in the set in ascending order.
   * This is just for compatibility with the Set<number> interface.
   *
   * WARNING: Is not allowed to change the bitmap while iterating. Undefined behaviour.
   * The iterator may throw exception if the bitmap is changed during the iteration.
   *
   * Same as [Symbol.iterator]()
   *
   * @returns {RoaringBitmap32Iterator} A new iterator
   * @memberof RoaringBitmap32
   */
  values(): RoaringBitmap32Iterator;

  /**
   * Overwrite the content of this bitmap copying it from an Iterable or another RoaringBitmap32.
   *
   * Is faster to pass a Uint32Array instance instead of an array or an iterable.
   * Is even faster if a RoaringBitmap32 instance is used (it performs a simple copy).
   *
   * @param {Iterable<number>} values The new values or a RoaringBitmap32 instance.
   * @returns {this} This RoaringBitmap32 instance.
   * @memberof RoaringBitmap32
   */
  copyFrom(values: Iterable<number> | null | undefined): this;

  /**
   * Adds the given value (or values) to the set.
   *
   * @param {...number[]} values The values to add.
   * @returns {this} This RoaringBitmap32 instance.
   * @memberof RoaringBitmap32
   */
  add(...values: (number | string | null | undefined)[]): this;

  /**
   * Tries to add the given value (or values) to the set.
   *
   * Returns true if the value (or at least one value) was added during this call, false if already existing or not a valid value.
   *
   * @param {...number[]} values The values to add.
   * @returns {boolean} True if operation was succesfull (values were added), false if not (values did not exists or are invalid values).
   * @memberof RoaringBitmap32
   */
  tryAdd(...values: (number | string | null | undefined)[]): boolean;

  /**
   * Adds multiple values to the set.
   *
   * Faster than calling add() multiple times.
   *
   * It is faster to insert sorted or partially sorted values.
   *
   * Is faster to use Uint32Array instead of arrays or iterables.
   *
   * Is optimized if the argument is an instance of RoaringBitmap32 (it performs an OR union).
   *
   * @param {Iterable<number>} values An iterable of values to insert.
   * @returns {this} This RoaringBitmap32 instance.
   * @memberof RoaringBitmap32
   */
  addMany(values: Iterable<number> | ArrayLike<number>): this;

  /**
   * Removes a value (or multiple values) from the set.
   *
   * Returns true if the value (or at least one value) was removed during this call, false if not.
   *
   * @param values The unsigned 32 bit values to remove.
   * @returns True if the value was removed during this call, false if not.
   * @memberof RoaringBitmap32
   */
  delete(...values: (number | string | null | undefined)[]): boolean;

  /**
   * Removes a value (or multiple values) from the set.
   *
   * Returns true if the value (or at least one value) was removed during this call, false if not.
   *
   * @param values The unsigned 32 bit values to remove.
   * @returns True if the value was removed during this call, false if not.
   * @memberof RoaringBitmap32
   */
  remove(...values: (number | string | null | undefined)[]): boolean;

  /**
   * The pop() method removes the last element from a roaring bitmap and returns that element.
   * This method changes the size of the bitmap
   * @returns {number | undefined} The last element in the bitmap, or undefined if the bitmap is empty.
   * @memberof RoaringBitmap32
   */
  pop(): number | undefined;

  /**
   * The shift() method removes the first element from a roaring bitmap and returns that element.
   * This method changes the size of the bitmap
   * @returns {number | undefined} The first element in the bitmap, or undefined if the bitmap is empty.
   * @memberof RoaringBitmap32
   */
  shift(): number | undefined;

  /**
   * Removes multiple values from the set.
   *
   * Faster than calling remove() multiple times.
   *
   * Is faster to use Uint32Array instead of arrays or iterables.
   *
   * This function is optimized if the argument is an instance of RoaringBitmap32 (it performs an AND NOT operation).
   *
   * @param values An iterable of values to remove.
   * @returns This RoaringBitmap32 instance.
   * @memberof RoaringBitmap32
   */
  removeMany(values: Iterable<number>): this;

  /**
   * Negates (in place) the roaring bitmap within a specified interval: [rangeStart, rangeEnd).
   *
   * First element is included, last element is excluded.
   * The number of negated values is rangeEnd - rangeStart.
   *
   * Areas outside the range are passed through unchanged.
   *
   * @param {number|undefined} rangeStart The start index. Trimmed to 0.
   * @param {number|undefined} [rangeEnd] The end index. Trimmed to 4294967296.
   * @returns {this} This RoaringBitmap32 instance.
   * @memberof RoaringBitmap32
   */
  flipRange(rangeStart: number | undefined, rangeEnd?: number | undefined): this;

  /**
   * Adds all the values in the interval: [rangeStart, rangeEnd).
   *
   * First element is included, last element is excluded.
   * The number of added values is rangeEnd - rangeStart.
   *
   * Areas outside the range are passed through unchanged.
   *
   * @param {number|undefined} rangeStart The start index. Trimmed to 0.
   * @param {number|undefined} [rangeEnd] The end index. Trimmed to 4294967296.
   * @returns {this} This RoaringBitmap32 instance.
   * @memberof RoaringBitmap32
   */
  addRange(rangeStart: number | undefined, rangeEnd?: number | undefined): this;

  /**
   * Removes all the values in the interval: [rangeStart, rangeEnd).
   *
   * First element is included, last element is excluded.
   * The number of renived values is rangeEnd - rangeStart.
   *
   * Areas outside the range are passed through unchanged.
   * @param {number} rangeStart The start index. Trimmed to 0.
   * @param {number} rangeEnd The end index. Trimmed to 4294967296.
   * @returns {this} This RoaringBitmap32 instance.
   * @memberof RoaringBitmap32
   */
  removeRange(rangeStart: number | undefined, rangeEnd?: number | undefined): this;

  /**
   * Performs an union in place ("this = this OR values"), same as addMany.
   *
   * Is faster to use Uint32Array instead of arrays or iterables.
   *
   * This function is optimized if the argument is an instance of RoaringBitmap32.
   *
   *
   * @param {Iterable<number>} values A RoaringBitmap32 instance or an iterable of unsigned 32 bit integers.
   * @returns {this} This RoaringBitmap32 instance.
   * @memberof RoaringBitmap32
   */
  orInPlace(values: Iterable<number>): this;

  /**
   * Performs a AND NOT operation in place ("this = this AND NOT values"), same as removeMany.
   *
   * Is faster to use Uint32Array instead of arrays or iterables.
   *
   * This function is optimized if the argument is an instance of RoaringBitmap32.
   *
   * @param {Iterable<number>} values A RoaringBitmap32 instance or an iterable of unsigned 32 bit integers.
   * @returns {this} This RoaringBitmap32 instance.
   * @memberof RoaringBitmap32
   */
  andNotInPlace(values: Iterable<number>): this;

  /**
   * Performs the intersection (and) between the current bitmap and the provided bitmap,
   * writing the result in the current bitmap.
   *
   * Is faster to use Uint32Array instead of arrays or iterables.
   *
   * This function is optimized if the argument is an instance of RoaringBitmap32.
   *
   * The provided bitmap is not modified.
   *
   * @param {Iterable<number>} values A RoaringBitmap32 instance or an iterable of unsigned 32 bit integers.
   * @returns {this} This RoaringBitmap32 instance.
   * @memberof RoaringBitmap32
   */
  andInPlace(values: Iterable<number>): this;

  /**
   * Performs the symmetric union (xor) between the current bitmap and the provided bitmap,
   * writing the result in the current bitmap.
   *
   * Is faster to use Uint32Array instead of arrays or iterables.
   *
   * This function is optimized if the argument is an instance of RoaringBitmap32.
   *
   * The provided bitmap is not modified.
   *
   * @param {Iterable<number>} values A RoaringBitmap32 instance or an iterable of unsigned 32 bit integers.
   * @returns {this} This RoaringBitmap32 instance.
   * @memberof RoaringBitmap32
   */
  xorInPlace(values: Iterable<number>): this;

  /**
   * Remove run-length encoding even when it is more space efficient.
   *
   * Return whether a change was applied.
   *
   * @returns {boolean} True if a change was applied, false if not.
   * @memberof RoaringBitmap32
   */
  removeRunCompression(): boolean;

  /**
   * Convert array and bitmap containers to run containers when it is more efficient;
   * also convert from run containers when more space efficient.
   *
   * Returns true if the bitmap has at least one run container.
   *
   * Additional savings might be possible by calling shrinkToFit().
   *
   * @returns {boolean} True if the bitmap has at least one run container.
   * @memberof RoaringBitmap32
   */
  runOptimize(): boolean;

  /**
   * If needed, reallocate memory to shrink the memory usage.
   *
   * Returns the number of bytes saved.
   *
   * @returns {number} The number of bytes saved.
   * @memberof RoaringBitmap32
   */
  shrinkToFit(): number;

  /**
   * Makes this roaring bitmap readonly.
   * Sets isFrozen to true.
   * This is a no-op if isFrozen is already true.
   * Every attempt to modify the bitmap will throw an exception.
   * A bitmap cannot be unfrozen.
   *
   * @returns {this} This instance.
   * @memberof RoaringBitmap32
   */
  freeze(): this;

  /**
   * A readonly view on this bitmap.
   * It returns always the same instance.
   * Can return this if this bitmap is fully hard frozen.
   * The bitmap will contain the same data, and if the first bitmap is modified
   * the readonlyView bitmap one will be modified too.
   * The readonlyView bitmap is readonly, so it is not possible to modify it.
   */
  asReadonlyView(): ReadonlyRoaringBitmap32;

  /**
   * Deserializes the bitmap from an Uint8Array or a Buffer.
   *
   * Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
   * The portable version is meant to be compatible with Java and Go versions.
   *
   * @param {Uint8Array | Int8Array | Uint8ClampedArray | ArrayBuffer|SharedArrayBuffer} serialized An Uint8Array or a node Buffer that contains the serialized data.
   * @param {DeserializationFormatType} format The format of the serialized data. true means "portable". false means "croaring".
   * @returns {this} This ReadonlyRoaringBitmap32 instance.
   * @memberof ReadonlyRoaringBitmap32
   */
  deserialize(
    serialized: Uint8Array | Int8Array | Uint8ClampedArray | ArrayBuffer | SharedArrayBuffer,
    format: DeserializationFormatType,
  ): this;

  /**
   * Checks wether the given value exists in the set.
   *
   * @param {number} value A 32 bit unsigned integer to search.
   * @returns {boolean} True if the set contains the given value, false if not.
   * @memberof ReadonlyRoaringBitmap32
   */
  has(value: unknown): boolean;
}

/**
 * Roaring bitmap that supports 32 bit unsigned integers.
 *
 * - See http://roaringbitmap.org/
 * - See https://github.com/SalvatorePreviti/roaring-node
 *
 * @export
 * @class RoaringBitmap32
 * @implements {Iterable<number>}
 * @author Salvatore Previti
 */
export class RoaringBitmap32 {
  // Allows: import RoaringBitmap32 from 'roaring/RoaringBitmap32'
  private static readonly default: typeof RoaringBitmap32;

  public static readonly SerializationFormat: typeof SerializationFormat;

  public readonly SerializationFormat: typeof SerializationFormat;

  public static readonly FileSerializationFormat: typeof FileSerializationFormat;

  public readonly FileSerializationFormat: typeof FileSerializationFormat;

  public static readonly FileDeserializationFormat: typeof FileDeserializationFormat;

  public readonly FileDeserializationFormat: typeof FileDeserializationFormat;

  public static readonly DeserializationFormat: typeof DeserializationFormat;

  public readonly DeserializationFormat: typeof DeserializationFormat;

  public static readonly FrozenViewFormat: typeof FrozenViewFormat;

  public readonly FrozenViewFormat: typeof FrozenViewFormat;

  /** Gets the approximate memory allocated by the roaring bitmap library. Useful for debugging memory issues and GC. */
  public static getRoaringUsedMemory(): number;

  /** Gets the total number of allocated RoaringBitmap32 globally. Useful for debugging memory issues and GC. */
  public static getInstancesCount(): number;

  /**
   * Creates a new buffer with the given size and alignment.
   * If alignment is not specified, the default alignment of 32 is used.
   * The buffer does not come from the nodejs buffer pool, it is allocated using aligned_malloc.
   *
   * Is the same as Buffer.alloc but is aligned.
   * We need an aligned buffer to create a roaring bitmap frozen view.
   *
   * @static
   * @param {number} size The size of the buffer to allocate.
   * @param {number} [alignment=32] The alignment of the buffer to allocate.
   * @memberof RoaringBitmap32
   */
  public static bufferAlignedAlloc(size: number, alignment?: number): Buffer;

  /**
   * Creates a new buffer not initialized with the given size and alignment.
   * If alignment is not specified, the default alignment of 32 is used.
   * The buffer does not come from the nodejs buffer pool, it is allocated using aligned_malloc.
   *
   * Is the same as Buffer.allocUnsafe but is aligned.
   * We need an aligned buffer to create a roaring bitmap frozen view.
   *
   * WARNING: this function is unsafe because the returned buffer may contain previously unallocated memory that may contain sensitive data.
   *
   * @static
   * @param {number} size The size of the buffer to allocate.
   * @param {number} [alignment=32] The alignment of the buffer to allocate.
   * @memberof RoaringBitmap32
   */
  public static bufferAlignedAllocUnsafe(size: number, alignment?: number): Buffer;

  /**
   * Creates a new buffer backed by a SharedArrayBuffer with the given size and alignment.
   * If alignment is not specified, the default alignment of 32 is used.
   * The buffer does not come from the nodejs buffer pool, it is allocated using aligned_malloc.
   *
   * Is the same as Buffer.alloc but is aligned and uses a SharedArrayBuffer as storage.
   * We need an aligned buffer to create a roaring bitmap frozen view.
   *
   * @param {number} size The size of the buffer to allocate.
   * @param {number} [alignment=32] The alignment of the buffer to allocate.
   */
  public static bufferAlignedAllocShared(size: number, alignment?: number): Buffer;

  /**
   * Creates a new buffer backed by a SharedArrayBuffer with the given size and alignment.
   * If alignment is not specified, the default alignment of 32 is used.
   * The buffer does not come from the nodejs buffer pool, it is allocated using aligned_malloc.
   *
   * Is the same as Buffer.allocUnsafe but is aligned and uses a SharedArrayBuffer as storage.
   * We need an aligned buffer to create a roaring bitmap frozen view.
   *
   * WARNING: this function is unsafe because the returned buffer may contain previously unallocated memory that may contain sensitive data.
   *
   * @param {number} size The size of the buffer to allocate.
   * @param {number} [alignment=32] The alignment of the buffer to allocate.
   */
  public static bufferAlignedAllocSharedUnsafe(size: number, alignment?: number): Buffer;

  /**
   * Ensures that the given buffer is aligned to the given alignment.
   * If alignment is not specified, the default alignment of 32 is used.
   * If the buffer is already aligned, it is returned.
   * If the buffer is not aligned, a new aligned buffer is created with bufferAlignedAllocUnsafe and the data is copied.
   *
   * @static
   * @param {Buffer} buffer The buffer to align.
   * @param {number} [alignment=32] The alignment to align to.
   * @returns {Buffer} The aligned buffer. Can be the same as the input buffer if it was already aligned. Can be a new buffer if the input buffer was not aligned.
   * @memberof RoaringBitmap32
   */
  public static ensureBufferAligned(
    buffer: Buffer | Uint8Array | Uint8ClampedArray | Int8Array | ArrayBuffer | SharedArrayBuffer,
    alignment?: number,
  ): Buffer;

  /**
   * Checks if the given buffer is memory aligned.
   * If alignment is not specified, the default alignment of 32 is used.
   *
   * @param {TypedArray | Buffer | ArrayBuffer|SharedArrayBuffer | null | undefined} buffer The buffer to check.
   * @param {number} [alignment=32] The alignment to check.
   */
  public static isBufferAligned(
    buffer: TypedArray | Buffer | ArrayBuffer | SharedArrayBuffer | null | undefined,
    alignment?: number,
  ): boolean;

  /**
   * Property: The version of the CRoaring libary as a string.
   * Example: "0.4.0"
   *
   * @export
   * @constant
   * @type {string} The version of the CRoaring libary as a string. Example: "0.2.42"
   */
  public static get CRoaringVersion(): string;

  /**
   * Property: The version of the roaring npm package as a string.
   * Example: "1.2.0"
   *
   * @export
   * @constant
   * @type {string} The version of the roaring npm package as a string. Example: "0.2.42"
   */
  public static get PackageVersion(): string;

  /**
   * Creates an instance of RoaringBitmap32 with a given initial capacity.
   *
   * @param {number} capacity The initial capacity of the bitmap.
   * @memberof RoaringBitmap32
   */
  public constructor(capacity?: number | undefined);

  /**
   * Creates an instance of RoaringBitmap32, or copies the given bitmap.
   *
   * Is faster to pass a Uint32Array instance instead of an array or an iterable.
   *
   * Is even faster if the given argument is a RoaringBitmap32 (performs a fast copy).
   *
   * @param {Iterable<number>} [values]
   * @memberof RoaringBitmap32
   */
  public constructor(values: Iterable<number>);

  /**
   * Creates a new frozen readonly view of the given bitmap.
   * The bitmap will contain the same data, and if the first bitmap is modified
   * the second one will be modified too.
   * The created bitmap is frozen, so it is not possible to modify it.
   *
   * @param {RoaringBitmap32} roaringBitmap32 The source RoaringBitmap32.
   * @param {"frozen"} frozen Must be "frozen"
   * @memberof RoaringBitmap32Iterator
   */
  public constructor(roaringBitmap32: ReadonlyRoaringBitmap32, frozen: "frozen");

  /**
   * Creates an instance of RoaringBitmap32 from the given Iterable.
   *
   * Is faster to pass a Uint32Array instance instead of an array or an iterable.
   *
   * Is optimized if the given argument is a RoaringBitmap32 (performs a fast copy).
   *
   * @static
   * @param {Iterable<number>} values The values to set.
   * @returns {RoaringBitmap32} A new RoaringBitmap32 instance filled with the given values.
   * @memberof RoaringBitmap32
   */
  public static from(values: Iterable<number>): RoaringBitmap32;

  /**
   * The RoaringBitmap32.of() static method creates a new Array instance from a variable number of arguments, regardless of number or type of the arguments.
   * Note that is faster to pass a Uint32Array instance instead of an array or an iterable.
   *
   * @static
   * @param {...number[]} values A set of values to add to the new RoaringBitmap32 instance.
   * @returns {RoaringBitmap32} A new RoaringBitmap32 instance.
   */
  public static of(...values: (number | string | null | undefined)[]): RoaringBitmap32;

  /**
   * Creates a new bitmap that contains all the values in the interval: [rangeStart, rangeEnd).
   * Is possible to specify the step parameter to have a non contiguous range.
   *
   * @static
   * @param {number|undefined} rangeStart The start index. Trimmed to 0.
   * @param {number|undefined} [rangeEnd] The end index. Trimmed to 4294967297.
   * @param {number|undefined} [step=1] The increment step, defaults to 1.
   * @returns {RoaringBitmap32} A new RoaringBitmap32 instance.
   * @memberof RoaringBitmap32
   */
  public static fromRange(
    rangeStart: number | undefined,
    rangeEnd?: number | undefined,
    step?: number | undefined,
  ): RoaringBitmap32;

  /**
   *
   * Creates an instance of RoaringBitmap32 from the given Iterable asynchronously in a parallel thread.
   *
   * If a plain array or a plain iterable is passed, a temporary Uint32Array will be created synchronously.
   *
   * NOTE: This method will throw a TypeError if a RoaringBitmap32 is passed as argument.
   *
   * Returns a Promise that resolves to a new RoaringBitmap32 instance.
   *
   * @static
   * @param {Iterable<number>} values The values to set. Cannot be a RoaringBitmap32.
   * @returns {Promise<RoaringBitmap32>} A promise that resolves to a new RoaringBitmap32 instance filled with all the given values.
   * @memberof RoaringBitmap32
   */
  public static fromArrayAsync(values: Iterable<number> | null | undefined): Promise<RoaringBitmap32>;

  /**
   *
   * Creates an instance of RoaringBitmap32 from the given Iterable asynchronously in a parallel thread.
   *
   * If a plain array or a plain iterable is passed, a temporary Uint32Array will be created synchronously.
   *
   * NOTE: This method will throw a TypeError if a RoaringBitmap32 is passed as argument.
   *
   * When deserialization is completed or failed, the given callback will be executed.
   *
   * @static
   * @param {Iterable<number>} values The values to set. Cannot be a RoaringBitmap32.
   * @param {RoaringBitmap32Callback} callback The callback to execute when the operation completes.
   * @returns {void}
   * @memberof RoaringBitmap32
   */
  public static fromArrayAsync(values: Iterable<number> | null | undefined, callback: RoaringBitmap32Callback): void;

  /**
   * Deserializes the bitmap from an Uint8Array or a Buffer.
   *
   * Returns a new RoaringBitmap32 instance.
   *
   * Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
   * The portable version is meant to be compatible with Java and Go versions.
   * When a frozen format is used, the buffer will be copied and the bitmap will be frozen.
   *
   * NOTE: this field was optional before, now is required and an Error is thrown if the portable flag is not passed.
   *
   * @static
   * @param {Uint8Array | Uint8ClampedArray | Int8Array | ArrayBuffer | SharedArrayBuffer | null | undefined} serialized An Uint8Array or a node Buffer that contains the serialized data.
   * @param {DeserializationFormatType} format The format of the serialized data. true means "portable". false means "croaring".
   * @returns {RoaringBitmap32} A new RoaringBitmap32 instance.
   * @memberof RoaringBitmap32
   */
  public static deserialize(
    serialized: Uint8Array | Uint8ClampedArray | Int8Array | ArrayBuffer | SharedArrayBuffer | null | undefined,
    format: DeserializationFormatType,
  ): RoaringBitmap32;

  /**
   *
   * Deserializes the bitmap from an Uint8Array or a Buffer asynchrnously in a parallel thread.
   *
   * Returns a Promise that resolves to a new RoaringBitmap32 instance.
   *
   * The portable version is meant to be compatible with Java and Go versions.
   * The croaring version is compatible with the C version, it can be smaller than the portable version.
   * When a frozen format is used, the buffer will be copied and the bitmap will be frozen.
   *
   * @static
   * @param {Uint8Array | Uint8ClampedArray | Int8Array | ArrayBuffer| SharedArrayBuffer | null | undefined} serialized An Uint8Array or a node Buffer that contains the serialized data.
   * @param {DeserializationFormatType} format The format of the serialized data. true means "portable". false means "croaring".
   * @returns {Promise<RoaringBitmap32>} A promise that resolves to a new RoaringBitmap32 instance.
   * @memberof RoaringBitmap32
   */
  public static deserializeAsync(
    serialized: Uint8Array | Uint8ClampedArray | Int8Array | ArrayBuffer | SharedArrayBuffer | null | undefined,
    format: DeserializationFormatType,
  ): Promise<RoaringBitmap32>;

  /**
   *
   * Deserializes the bitmap from an Uint8Array or a Buffer asynchrnously in a parallel thread.
   *
   * When deserialization is completed or failed, the given callback will be executed.
   *
   * The portable version is meant to be compatible with Java and Go versions.
   * The croaring version is compatible with the C version, it can be smaller than the portable version.
   * When a frozen format is used, the buffer will be copied and the bitmap will be frozen.
   *
   * @static
   * @param {Uint8Array | Uint8ClampedArray | Int8Array | ArrayBuffer| SharedArrayBuffer | null | undefined} serialized An Uint8Array or a node Buffer that contains the.
   * @param {DeserializationFormatType} format The format of the serialized data. true means "portable". false means "croaring".
   * @param {RoaringBitmap32Callback} callback The callback to execute when the operation completes.
   * @returns {void}
   * @memberof RoaringBitmap32
   */
  public static deserializeAsync(
    serialized: Uint8Array | Uint8ClampedArray | Int8Array | ArrayBuffer | SharedArrayBuffer | null | undefined,
    format: DeserializationFormatType,
    callback: RoaringBitmap32Callback,
  ): void;

  /**
   * Deserializes the bitmap from a file asynchronously.
   * Returns a new RoaringBitmap32 instance.
   *
   * The portable version is meant to be compatible with Java and Go versions.
   * The croaring version is compatible with the C version, it can be smaller than the portable version.
   * When a frozen format is used, the buffer will be copied and the bitmap will be frozen.
   *
   * This is faster, everything runs in its own thread and it consumes less memory than serializing to a Buffer and then to write to a file,
   * internally it uses memory mapped files and skip all the JS overhead.
   *
   * @static
   * @param {string} filePath The path of the file to read.
   * @param {FileDeserializationFormatType} format The format of the serialized data. true means "portable". false means "croaring".
   * @returns {Promise<RoaringBitmap32>} A promise that resolves to a new RoaringBitmap32 instance.
   * @memberof RoaringBitmap32
   */
  public static deserializeFileAsync(filePath: string, format: FileDeserializationFormatType): Promise<RoaringBitmap32>;

  /**
   *
   * Deserializes many bitmaps from an array of Uint8Array or an array of Buffer asynchronously in multiple parallel threads.
   *
   * Returns a Promise that resolves to an array of new RoaringBitmap32 instance.
   *
   * Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
   * The portable version is meant to be compatible with Java and Go versions.
   * When a frozen format is used, the buffer will be copied and the bitmap will be frozen.
   *
   * NOTE: portable argument was optional before, now is required and an Error is thrown if the portable flag is not passed.
   *
   * @static
   * @param {(Uint8Array | Uint8ClampedArray | Int8Array | ArrayBuffer| SharedArrayBuffer | null | undefined)[]} serialized An Uint8Array or a node Buffer that contains the serialized data.
   * @param {DeserializationFormatType} format The format of the serialized data. true means "portable". false means "croaring".
   * @returns {Promise<RoaringBitmap32[]>} A promise that resolves to a new RoaringBitmap32 instance.
   * @memberof RoaringBitmap32
   */
  public static deserializeParallelAsync(
    serialized: (Uint8Array | Uint8ClampedArray | Int8Array | ArrayBuffer | SharedArrayBuffer | null | undefined)[],
    format: DeserializationFormatType,
  ): Promise<RoaringBitmap32[]>;

  /**
   *
   * Deserializes many bitmaps from an array of Uint8Array or an array of Buffer asynchronously in a parallel thread.
   *
   * Deserialization in the parallel thread will be executed in sequence, if one fails, all fails.
   *
   * Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
   * The portable version is meant to be compatible with Java and Go versions.
   * When a frozen format is used, the buffer will be copied and the bitmap will be frozen.
   *
   * NOTE: portable argument was optional before, now is required and an Error is thrown if the portable flag is not passed.
   *
   * When deserialization is completed or failed, the given callback will be executed.
   *
   * @static
   * @param {Uint8Array[]} serialized An array of Uint8Array or node Buffers that contains the non portable serialized data.
   * @param {DeserializationFormatType} format The format of the serialized data. true means "portable". false means "croaring".
   * @param {RoaringBitmap32ArrayCallback} callback The callback to execute when the operation completes.
   * @returns {void}
   * @memberof RoaringBitmap32
   */
  public static deserializeParallelAsync(
    serialized: readonly (Uint8Array | Uint8ClampedArray | Int8Array | ArrayBuffer | SharedArrayBuffer)[],
    format: DeserializationFormatType,
    callback: RoaringBitmap32ArrayCallback,
  ): void;

  /**
   * This is an unsafe method that builds a frozen roaring bitmap over a buffer.
   * This removes the overhead of a deserialization of a large bitmap.
   * The buffer will be kept alive by the bitmap.
   * This function is endian sensitive, it supports only little endian and cannot load frozen bitmaps saved on big endian machines.
   *
   * This function is considered unsafe because if the buffer gets modified, the bitmap will be corrupted and the application can crash.
   * There is a risk for buffer overrun or arbitrary code execution here. Be careful and do not allow the buffer to be modified while or after the bitmap is in use.
   *
   * The content of buffer should not be modified until the bitmap is destroyed! Is responsibility of the caller to ensure that the buffer is not modified while the bitmap is in use.
   *
   * Using "unsafe_frozen_croaring" the buffer data must be aligned to 32 bytes.
   * The roaring library provides the functions bufferAlignedAlloc, bufferAlignedAllocUnsafe, isBufferAligned, ensureBufferAligned that helps allocating and managing aligned buffers.
   * If you read a file, or read from database, be careful to use an aligned buffer or copy to an aligned buffer before calling this function with "unsafe_frozen_croaring" format.
   *
   * Is considered unsafe and unstable because the format might change at any new version.
   * Can be useful for temporary storage or for sending data over the network between similar machines.
   * If the content is corrupted when loaded or the buffer is modified when a frozen view is create, the behavior is undefined!
   * The application may crash, buffer overrun, could be a vector of attack!
   *
   * @static
   * @param {Buffer} storage A Buffer that contains the serialized data.
   * @param {FrozenViewFormatType} format The format of the serialized data. true means "portable". false means "croaring".
   * @returns {RoaringBitmap32} A new RoaringBitmap32 instance.
   * @memberof RoaringBitmap32
   */
  public static unsafeFrozenView(
    storage: Buffer | Uint8Array | Uint8ClampedArray | Int8Array | ArrayBuffer | SharedArrayBuffer,
    format: FrozenViewFormatType,
  ): RoaringBitmap32;

  /**
   * Swaps the content of two RoaringBitmap32 instances.
   *
   * @static
   * @param {RoaringBitmap32} a First RoaringBitmap32 instance to swap
   * @param {RoaringBitmap32} b Second RoaringBitmap32 instance to swap
   * @memberof RoaringBitmap32
   */
  public static swap(a: RoaringBitmap32, b: RoaringBitmap32): void;

  /**
   * addOffset adds the value 'offset' to each and every value in a bitmap, generating a new bitmap in the process.
   * If offset + element is outside of the range [0,2^32), that the element will be dropped.
   *
   * @param {ReadonlyRoaringBitmap32} input The input bitmap.
   * @param {number} offset The offset to add to each element. Can be positive or negative.
   * @returns {RoaringBitmap32} A new bitmap with the offset added to each element.
   */
  public static addOffset(input: ReadonlyRoaringBitmap32, offset: number): RoaringBitmap32;

  /**
   * Returns a new RoaringBitmap32 with the intersection (and) between the given two bitmaps.
   *
   * The provided bitmaps are not modified.
   *
   * @static
   * @param {ReadonlyRoaringBitmap32} a The first RoaringBitmap32 instance to and.
   * @param {ReadonlyRoaringBitmap32} b The second RoaringBitmap32 instance to and.
   * @returns {RoaringBitmap32} A new RoaringBitmap32 that contains the intersection a AND b
   * @memberof RoaringBitmap32
   */
  public static and(a: ReadonlyRoaringBitmap32, b: ReadonlyRoaringBitmap32): RoaringBitmap32;

  /**
   * Returns a new RoaringBitmap32 with the union (or) of the two given bitmaps.
   *
   * The provided bitmaps are not modified.
   *
   * @static
   * @param {ReadonlyRoaringBitmap32} a The first RoaringBitmap32 instance to or.
   * @param {ReadonlyRoaringBitmap32} b The second RoaringBitmap32 instance to or.
   * @returns {RoaringBitmap32}
   * @memberof RoaringBitmap32 A new RoaringBitmap32 that contains the union a OR b
   */
  public static or(a: ReadonlyRoaringBitmap32, b: ReadonlyRoaringBitmap32): RoaringBitmap32;

  /**
   * Returns a new RoaringBitmap32 with the symmetric union (xor) between the two given bitmaps.
   *
   * The provided bitmaps are not modified.
   *
   * @static
   * @param {ReadonlyRoaringBitmap32} a The first RoaringBitmap32 instance to xor.
   * @param {ReadonlyRoaringBitmap32} b The second RoaringBitmap32 instance to xor.
   * @returns {RoaringBitmap32}
   * @memberof RoaringBitmap32 A new RoaringBitmap32 that contains a XOR b
   */
  public static xor(a: ReadonlyRoaringBitmap32, b: ReadonlyRoaringBitmap32): RoaringBitmap32;

  /**
   * Returns a new RoaringBitmap32 with the difference (and not) between the two given bitmaps.
   *
   * The provided bitmaps are not modified.
   *
   * @static
   * @param {ReadonlyRoaringBitmap32} a The first RoaringBitmap32 instance.
   * @param {ReadonlyRoaringBitmap32} b The second RoaringBitmap32 instance.
   * @returns {RoaringBitmap32}
   * @memberof RoaringBitmap32 A new bitmap, a AND NOT b
   */
  public static andNot(a: ReadonlyRoaringBitmap32, b: ReadonlyRoaringBitmap32): RoaringBitmap32;

  /**
   * Performs a union between all the given array of RoaringBitmap32 instances.
   *
   * This function is faster than calling or multiple times.
   *
   * @static
   * @param {ReadonlyRoaringBitmap32[]} bitmaps An array of RoaringBitmap32 instances to or together.
   * @returns {RoaringBitmap32} A new RoaringBitmap32 that contains the union of all the given bitmaps.
   * @memberof RoaringBitmap32
   */
  public static orMany(bitmaps: readonly ReadonlyRoaringBitmap32[]): RoaringBitmap32;

  /**
   * Performs a union between all the given RoaringBitmap32 instances.
   *
   * This function is faster than calling or multiple times.
   *
   * @static
   * @param {...ReadonlyRoaringBitmap32[]} bitmaps The RoaringBitmap32 instances to or together.
   * @returns {RoaringBitmap32} A new RoaringBitmap32 that contains the union of all the given bitmaps.
   * @memberof RoaringBitmap32
   */
  public static orMany(...bitmaps: readonly ReadonlyRoaringBitmap32[]): RoaringBitmap32;

  /**
   * Performs a xor between all the given array of RoaringBitmap32 instances.
   *
   * This function is faster than calling xor multiple times.
   *
   * @static
   * @param {ReadonlyRoaringBitmap32[]} bitmaps An array of RoaringBitmap32 instances to or together.
   * @returns {RoaringBitmap32} A new RoaringBitmap32 that contains the xor of all the given bitmaps.
   * @memberof RoaringBitmap32
   */
  public static xorMany(bitmaps: readonly ReadonlyRoaringBitmap32[]): RoaringBitmap32;

  /**
   * Performs a xor between all the given RoaringBitmap32 instances.
   *
   * This function is faster than calling xor multiple times.
   *
   * @static
   * @param {...ReadonlyRoaringBitmap32[]} bitmaps The RoaringBitmap32 instances to or together.
   * @returns {RoaringBitmap32} A new RoaringBitmap32 that contains the xor of all the given bitmaps.
   * @memberof RoaringBitmap32
   */
  public static xorMany(...bitmaps: readonly ReadonlyRoaringBitmap32[]): RoaringBitmap32;
}

/**
 * Iterator for RoaringBitmap32.
 *
 * WARNING: Is not allowed to change the bitmap while iterating.
 * The iterator may throw exception if the bitmap is changed during the iteration.
 *
 * @export
 * @class RoaringBitmap32Iterator
 * @implements {IterableIterator<number>}
 */
export class RoaringBitmap32Iterator implements IterableIterator<number> {
  get isReverseIterator(): boolean;

  // Allows: import RoaringBitmap32Iterator from 'roaring/RoaringBitmap32Iterator'
  private static readonly default: typeof RoaringBitmap32Iterator;

  /**
   * Creates a new iterator able to iterate a RoaringBitmap32.
   *
   * @param {RoaringBitmap32} [roaringBitmap32] The roaring bitmap to iterate. If null or undefined, an empty iterator is created.
   * @memberof RoaringBitmap32Iterator
   */
  public constructor(roaringBitmap32?: ReadonlyRoaringBitmap32, reverse?: boolean);

  /**
   * Creates a new iterator able to iterate a RoaringBitmap32.
   *
   * It allocates a small temporary buffer of the given size for speedup.
   *
   * @param {ReadonlyRoaringBitmap32} roaringBitmap32 The roaring bitmap to iterate
   * @param {number} bufferSize Buffer size to allocate, must be an integer greater than 0
   * @memberof RoaringBitmap32Iterator
   */
  public constructor(roaringBitmap32: ReadonlyRoaringBitmap32, bufferSize: number);

  /**
   * Creates a new iterator able to iterate a RoaringBitmap32 using the given temporary buffer.
   *
   * @param {ReadonlyRoaringBitmap32} roaringBitmap32
   * @param {Uint32Array} buffer The roaring bitmap to iterate
   * @memberof RoaringBitmap32Iterator The reusable temporary buffer. Length must be greater than 0.
   */
  public constructor(roaringBitmap32: ReadonlyRoaringBitmap32, buffer: Uint32Array);

  /**
   * Returns this.
   *
   * @returns {this} The same instance.
   * @memberof RoaringBitmap32Iterator
   */
  public [Symbol.iterator](): this;

  /**
   * Returns the next element in the iterator.
   *
   * For performance reasons, this function returns always the same instance.
   *
   * @returns {IteratorResult<number>} The next result.
   * @memberof RoaringBitmap32Iterator
   */
  public next(): IteratorResult<number>;
}

/**
 * Reverse iterator for RoaringBitmap32.
 *
 * WARNING: Is not allowed to change the bitmap while iterating.
 * The iterator may throw exception if the bitmap is changed during the iteration.
 *
 * @export
 * @class RoaringBitmap32ReverseIterator
 * @implements {IterableIterator<number>}
 */
export class RoaringBitmap32ReverseIterator implements IterableIterator<number> {
  // Allows: import RoaringBitmap32ReverseIterator from 'roaring/RoaringBitmap32ReverseIterator'
  private static readonly default: typeof RoaringBitmap32ReverseIterator;

  /**
   * Creates a new iterator able to iterate a RoaringBitmap32.
   *
   * @param {RoaringBitmap32} [roaringBitmap32] The roaring bitmap to iterate. If null or undefined, an empty iterator is created.
   * @memberof RoaringBitmap32ReverseIterator
   */
  public constructor(roaringBitmap32?: ReadonlyRoaringBitmap32);

  /**
   * Creates a new iterator able to iterate a RoaringBitmap32.
   *
   * It allocates a small temporary buffer of the given size for speedup.
   *
   * @param {ReadonlyRoaringBitmap32} roaringBitmap32 The roaring bitmap to iterate
   * @param {number} bufferSize Buffer size to allocate, must be an integer greater than 0
   * @memberof RoaringBitmap32ReverseIterator
   */
  public constructor(roaringBitmap32: ReadonlyRoaringBitmap32, bufferSize: number);

  /**
   * Creates a new iterator able to iterate a RoaringBitmap32 using the given temporary buffer.
   *
   * @param {ReadonlyRoaringBitmap32} roaringBitmap32
   * @param {Uint32Array} buffer The roaring bitmap to iterate
   * @memberof RoaringBitmap32ReverseIterator The reusable temporary buffer. Length must be greater than 0.
   */
  public constructor(roaringBitmap32: ReadonlyRoaringBitmap32, buffer: Uint32Array);

  /**
   * Returns this.
   *
   * @returns {this} The same instance.
   * @memberof RoaringBitmap32ReverseIterator
   */
  public [Symbol.iterator](): this;

  /**
   * Returns the next element in the iterator.
   *
   * For performance reasons, this function returns always the same instance.
   *
   * @returns {IteratorResult<number>} The next result.
   * @memberof RoaringBitmap32ReverseIterator
   */
  public next(): IteratorResult<number>;
}

/**
 * Object returned by RoaringBitmap32 statistics() method
 *
 * @export
 * @interface RoaringBitmap32Statistics
 */
export interface RoaringBitmap32Statistics {
  /**
   * Number of containers.
   * @type {number}
   */
  containers: number;

  /**
   * Number of array containers.
   * @type {number}
   */
  arrayContainers: number;

  /**
   * Number of run containers.
   * @type {number}
   */
  runContainers: number;

  /**
   * Number of bitmap containers.
   * @type {number}
   */
  bitsetContainers: number;

  /**
   * Number of values in array containers.
   * @type {number}
   */
  valuesInArrayContainers: number;

  /**
   * Number of values in run containers.
   * @type {number}
   */
  valuesInRunContainers: number;

  /**
   * Number of values in  bitmap containers.
   * @type {number}
   */
  valuesInBitsetContainers: number;

  /**
   * Number of allocated bytes in array containers.
   * @type {number}
   */
  bytesInArrayContainers: number;

  /**
   * Number of allocated bytes in run containers.
   * @type {number}
   */
  bytesInRunContainers: number;

  /**
   * Number of allocated bytes in bitmap containers.
   * @type {number}
   */
  bytesInBitsetContainers: number;

  /**
   * The maximal value.
   * @type {number}
   */
  maxValue: number;

  /**
   * The minimal value.
   * @type {number}
   */
  minValue: number;

  /**
   * The sum of all values
   * @type {number}
   */
  sumOfAllValues: number;

  /**
   * Total number of values stored in the bitmap
   * @type {number}
   */
  size: number;

  /**
   * True if this bitmap is frozen and cannot be modified
   * @type {boolean}
   */
  isFrozen: boolean;
}

/**
 * Property: The version of the CRoaring libary as a string.
 * Example: "0.4.0"
 *
 * @export
 * @constant
 * @type {string} The version of the CRoaring libary as a string. Example: "0.2.42"
 * @memberof RoaringModule
 */
export const CRoaringVersion: string;

/**
 * Property: The version of the roaring npm package as a string.
 * Example: "1.2.0"
 *
 * @export
 * @constant
 * @type {string} The version of the roaring npm package as a string. Example: "0.2.42"
 * @memberof RoaringModule
 */
export const PackageVersion: string;

export default roaring;

export type RoaringBitmap32Callback = (error: Error | null, bitmap: RoaringBitmap32 | undefined) => void;

export type RoaringBitmap32ArrayCallback = (error: Error | null, bitmap: RoaringBitmap32[] | undefined) => void;

// tslint:disable-next-line:no-empty-interface
declare interface Buffer extends Uint8Array {}
