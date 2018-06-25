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
*/

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
export class RoaringBitmap32 implements Iterable<number> {
  // Allows: import RoaringBitmap32 from 'roaring/RoaringBitmap32'
  private static readonly default: typeof RoaringBitmap32

  /**
   * Property. Gets the number of items in the set (cardinality).
   *
   * @type {number}
   * @memberof RoaringBitmap32
   */
  public readonly size: number

  /**
   * Property. True if the bitmap is empty.
   *
   * @type {boolean}
   * @memberof RoaringBitmap32
   */
  public readonly isEmpty: boolean

  /**
   * Creates an instance of RoaringBitmap32.
   *
   * Is faster to pass a Uint32Array instance instead of an array or an iterable.
   *
   * Is even faster if the given argument is a RoaringBitmap32 (performs a fast copy).
   *
   * @param {Iterable<number>} [values]
   * @memberof RoaringBitmap32
   */
  public constructor(values?: Iterable<number>)

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
  public static from(values: Iterable<number>): RoaringBitmap32

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
  public static fromArrayAsync(values: Iterable<number> | null | undefined): Promise<RoaringBitmap32>

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
  public static fromArrayAsync(values: Iterable<number> | null | undefined, callback: RoaringBitmap32Callback): void

  /**
   * Deserializes the bitmap from an Uint8Array or a Buffer.
   *
   * Returns a new RoaringBitmap32 instance.
   *
   * Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
   * The portable version is meant to be compatible with Java and Go versions.
   *
   * @static
   * @param {Uint8Array} serialized An Uint8Array or a node Buffer that contains the serialized data.
   * @param {boolean} [portable] If false (default), optimized C/C++ format is used.  If true, Java and Go portable format is used.
   * @returns {RoaringBitmap32} A new RoaringBitmap32 instance.
   * @memberof RoaringBitmap32
   */
  public static deserialize(serialized: Uint8Array, portable?: boolean): RoaringBitmap32

  /**
   *
   * Deserializes the bitmap from an Uint8Array or a Buffer asynchrnously in a parallel thread.
   *
   * Returns a Promise that resolves to a new RoaringBitmap32 instance.
   *
   * Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
   * The portable version is meant to be compatible with Java and Go versions.
   *
   * @static
   * @param {Uint8Array} serialized An Uint8Array or a node Buffer that contains the serialized data.
   * @param {boolean} [portable] If false (default), optimized C/C++ format is used.  If true, Java and Go portable format is used.
   * @returns {Promise<RoaringBitmap32>} A promise that resolves to a new RoaringBitmap32 instance.
   * @memberof RoaringBitmap32
   */
  public static deserializeAsync(serialized: Uint8Array, portable?: boolean): Promise<RoaringBitmap32>

  /**
   *
   * Deserializes the bitmap from an Uint8Array or a Buffer asynchrnously in a parallel thread.
   *
   * When deserialization is completed or failed, the given callback will be executed.
   *
   * @static
   * @param {Uint8Array} serialized An Uint8Array or a node Buffer that contains the non portable serialized data.
   * @param {RoaringBitmap32Callback} callback The callback to execute when the operation completes.
   * @returns {void}
   * @memberof RoaringBitmap32
   */
  public static deserializeAsync(serialized: Uint8Array, callback: RoaringBitmap32Callback): void

  /**
   *
   * Deserializes the bitmap from an Uint8Array or a Buffer asynchrnously in a parallel thread.
   *
   * When deserialization is completed or failed, the given callback will be executed.
   *
   * Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
   * The portable version is meant to be compatible with Java and Go versions.
   *
   * @static
   * @param {Uint8Array} serialized An Uint8Array or a node Buffer that contains the.
   * @param {boolean} [portable] If false (default), optimized C/C++ format is used.  If true, Java and Go portable format is used.
   * @param {RoaringBitmap32Callback} callback The callback to execute when the operation completes.
   * @returns {void}
   * @memberof RoaringBitmap32
   */
  public static deserializeAsync(serialized: Uint8Array, portable: boolean, callback: RoaringBitmap32Callback): void

  /**
   *
   * Deserializes many bitmaps from an array of Uint8Array or an array of Buffer asynchronously in multiple parallel threads.
   *
   * Returns a Promise that resolves to an array of new RoaringBitmap32 instance.
   *
   * Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
   * The portable version is meant to be compatible with Java and Go versions.
   *
   * @static
   * @param {Uint8Array[]} serialized An Uint8Array or a node Buffer that contains the serialized data.
   * @param {boolean} [portable] If false (default), optimized C/C++ format is used.  If true, Java and Go portable format is used.
   * @returns {Promise<RoaringBitmap32[]>} A promise that resolves to a new RoaringBitmap32 instance.
   * @memberof RoaringBitmap32
   */
  public static deserializeParallelAsync(serialized: (Uint8Array | null | undefined)[], portable?: boolean): Promise<RoaringBitmap32[]>

  /**
   *
   * Deserializes many bitmaps from an array of Uint8Array or an array of Buffer asynchronously in multiple parallel threads.
   *
   * When deserialization is completed or failed, the given callback will be executed.
   *
   * @static
   * @param {Uint8Array[]} serialized An array of Uint8Array or node Buffers that contains the non portable serialized data.
   * @param {RoaringBitmap32ArrayCallback} callback The callback to execute when the operation completes.
   * @returns {void}
   * @memberof RoaringBitmap32
   */
  public static deserializeParallelAsync(serialized: (Uint8Array | null | undefined)[], callback: RoaringBitmap32ArrayCallback): void

  /**
   *
   * Deserializes many bitmaps from an array of Uint8Array or an array of Buffer asynchronously in a parallel thread.
   *
   * Deserialization in the parallel thread will be executed in sequence, if one fails, all fails.
   *
   * Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
   * The portable version is meant to be compatible with Java and Go versions.
   *
   * When deserialization is completed or failed, the given callback will be executed.
   *
   * @static
   * @param {Uint8Array[]} serialized An array of Uint8Array or node Buffers that contains the non portable serialized data.
   * @param {RoaringBitmap32ArrayCallback} callback The callback to execute when the operation completes.
   * @returns {void}
   * @memberof RoaringBitmap32
   */
  public static deserializeParallelAsync(
    serialized: (Uint8Array | null | undefined)[],
    portable: boolean,
    callback: RoaringBitmap32ArrayCallback
  ): void

  /**
   * Swaps the content of two RoaringBitmap32 instances.
   *
   * @static
   * @param {RoaringBitmap32} a First RoaringBitmap32 instance to swap
   * @param {RoaringBitmap32} b Second RoaringBitmap32 instance to swap
   * @memberof RoaringBitmap32
   */
  public static swap(a: RoaringBitmap32, b: RoaringBitmap32): void

  /**
   * Returns a new RoaringBitmap32 with the intersection (and) between the given two bitmaps.
   *
   * The provided bitmaps are not modified.
   *
   * @static
   * @param {RoaringBitmap32} a The first RoaringBitmap32 instance to and.
   * @param {RoaringBitmap32} b The second RoaringBitmap32 instance to and.
   * @returns {RoaringBitmap32} A new RoaringBitmap32 that contains the intersection a AND b
   * @memberof RoaringBitmap32
   */
  public static and(a: RoaringBitmap32, b: RoaringBitmap32): RoaringBitmap32

  /**
   * Returns a new RoaringBitmap32 with the union (or) of the two given bitmaps.
   *
   * The provided bitmaps are not modified.
   *
   * @static
   * @param {RoaringBitmap32} a The first RoaringBitmap32 instance to or.
   * @param {RoaringBitmap32} b The second RoaringBitmap32 instance to or.
   * @returns {RoaringBitmap32}
   * @memberof RoaringBitmap32 A new RoaringBitmap32 that contains the union a OR b
   */
  public static or(a: RoaringBitmap32, b: RoaringBitmap32): RoaringBitmap32

  /**
   * Returns a new RoaringBitmap32 with the symmetric union (xor) between the two given bitmaps.
   *
   * The provided bitmaps are not modified.
   *
   * @static
   * @param {RoaringBitmap32} a The first RoaringBitmap32 instance to xor.
   * @param {RoaringBitmap32} b The second RoaringBitmap32 instance to xor.
   * @returns {RoaringBitmap32}
   * @memberof RoaringBitmap32 A new RoaringBitmap32 that contains a XOR b
   */
  public static xor(a: RoaringBitmap32, b: RoaringBitmap32): RoaringBitmap32

  /**
   * Returns a new RoaringBitmap32 with the difference (and not) between the two given bitmaps.
   *
   * The provided bitmaps are not modified.
   *
   * @static
   * @param {RoaringBitmap32} a The first RoaringBitmap32 instance.
   * @param {RoaringBitmap32} b The second RoaringBitmap32 instance.
   * @returns {RoaringBitmap32}
   * @memberof RoaringBitmap32 A new bitmap, a AND NOT b
   */
  public static andNot(a: RoaringBitmap32, b: RoaringBitmap32): RoaringBitmap32

  /**
   * Performs a union between all the given array of RoaringBitmap32 instances.
   *
   * This function is faster than calling or multiple times.
   *
   * @static
   * @param {RoaringBitmap32[]} values An array of RoaringBitmap32 instances to or together.
   * @returns {RoaringBitmap32} A new RoaringBitmap32 that contains the union of all the given bitmaps.
   * @memberof RoaringBitmap32
   */
  public static orMany(values: RoaringBitmap32[]): RoaringBitmap32

  /**
   * Performs a union between all the given RoaringBitmap32 instances.
   *
   * This function is faster than calling or multiple times.
   *
   * @static
   * @param {...RoaringBitmap32[]} values The RoaringBitmap32 instances to or together.
   * @returns {RoaringBitmap32} A new RoaringBitmap32 that contains the union of all the given bitmaps.
   * @memberof RoaringBitmap32
   */
  public static orMany(...values: RoaringBitmap32[]): RoaringBitmap32

  /**
   * [Symbol.iterator]() Gets a new iterator able to iterate all values in the set in order.
   *
   * @returns {RoaringBitmap32Iterator} A new iterator
   * @memberof RoaringBitmap32
   */
  public [Symbol.iterator](): RoaringBitmap32Iterator

  /**
   * Gets a new iterator able to iterate all values in the set in order.
   *
   * Same as [Symbol.iterator]()
   *
   * @returns {RoaringBitmap32Iterator} A new iterator
   * @memberof RoaringBitmap32
   */
  public iterator(): RoaringBitmap32Iterator

  /**
   * Gets the minimum value in the set.
   *
   * @returns {number} The minimum value in the set or 0xFFFFFFFF if empty.
   * @memberof RoaringBitmap32
   */
  public minimum(): number

  /**
   * Gets the maximum value in the set.
   *
   * @returns {number} The minimum value in the set or 0 if empty.
   * @memberof RoaringBitmap32
   */
  public maximum(): number

  /**
   * Checks wether the given value exists in the set.
   *
   * @param {number} value A 32 bit unsigned integer to search.
   * @returns {boolean} True if the set contains the given value, false if not.
   * @memberof RoaringBitmap32
   */
  public has(value: number): boolean

  /**
   * Check whether a range of values from range_start (included) to range_end (excluded) is present
   *
   * @param {number} rangeStart The start index.
   * @param {number} rangeEnd The end index.
   * @returns {boolean} True if the bitmap contains the whole range of values from range_start (included) to range_end (excluded), false if not.
   * @memberof RoaringBitmap32
   */
  public hasRange(rangeStart: number, rangeEnd: number): boolean

  /**
   * Overwrite the content of this bitmap copying it from an Iterable or another RoaringBitmap32.
   *
   * Is faster to pass a Uint32Array instance instead of an array or an iterable.
   *
   * Is even faster if a RoaringBitmap32 instance is used (it performs a simple copy).
   *
   * @param {Iterable<number>} values The new values or a RoaringBitmap32 instance.
   * @memberof RoaringBitmap32
   */
  public copyFrom(values: Iterable<number> | null | undefined): void

  /**
   * Adds a single value to the set.
   *
   * @param {number} value The value to add. Must be a valid 32 bit unsigned integer.
   * @returns {this} This RoaringBitmap32 instance.
   * @memberof RoaringBitmap32
   */
  public add(value: number): this

  /**
   * Tries to add a single value to the set.
   *
   * Returns true if the value was added during this call, false if already existing or not a valid unsigned integer 32.
   *
   * @param {number} value The value to add. Must be a valid 32 bit unsigned integer.
   * @returns {boolean} True if operation was succesfull (value added), false if not (value does not exists or is not a valid 32 bit unsigned integer).
   * @memberof RoaringBitmap32
   */
  public tryAdd(value: number): boolean

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
  public addMany(values: Iterable<number>): this

  /**
   * Removes a value from the set.
   *
   * Returns true if the value was removed during this call, false if not.
   *
   * @param value The unsigned 32 bit integer to remove.
   * @returns True if the value was removed during this call, false if not.
   */
  public delete(value: number): boolean

  /**
   * Removes a value from the set.
   *
   * @param value The unsigned 32 bit integer to remove.
   */
  public remove(value: number): void

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
   */
  public removeMany(values: Iterable<number>): this

  /**
   * Negates (in place) the roaring bitmap within a specified interval: [rangeStart, rangeEnd).
   *
   * First element is included, last element is excluded.
   * The number of negated values is rangeEnd - rangeStart.
   *
   * Areas outside the range are passed through unchanged.
   *
   * @param rangeStart The start index. Trimmed to 0.
   * @param rangeEnd The end index. Trimmed to 4294967297.
   */
  public flipRange(rangeStart: number, rangeEnd: number): void

  /**
   * Adds all the values in the interval: [rangeStart, rangeEnd).
   *
   * First element is included, last element is excluded.
   * The number of added values is rangeEnd - rangeStart.
   *
   * Areas outside the range are passed through unchanged.
   *
   * @param {number} rangeStart The start index. Trimmed to 0.
   * @param {number} rangeEnd The end index. Trimmed to 4294967297.
   * @memberof RoaringBitmap32
   */
  public addRange(rangeStart: number, rangeEnd: number): void

  /**
   * Removes all values from the set.
   * It frees resources, if needed you can use clear to free some memory before the garbage collector disposes this instance.
   *
   * @memberof RoaringBitmap32
   */
  public clear(): void

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
  public orInPlace(values: Iterable<number>): this

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
  public andNotInPlace(values: Iterable<number>): this

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
  public andInPlace(values: Iterable<number>): this

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
  public xorInPlace(values: Iterable<number>): this

  /**
   * Checks wether this set is a subset or the same as the given set.
   *
   * Returns false also if the given argument is not a RoaringBitmap32 instance.
   *
   * @param {RoaringBitmap32} other The other set.
   * @returns {boolean} True if this set is a subset of the given RoaringBitmap32. False if not.
   * @memberof RoaringBitmap32
   */
  public isSubset(other: RoaringBitmap32): boolean

  /**
   * Checks wether this set is a strict subset of the given set.
   *
   * Returns false if the sets are the same.
   *
   * Returns false also if the given argument is not a RoaringBitmap32 instance.
   *
   * @param {RoaringBitmap32} other The other RoaringBitmap32 instance.
   * @returns {boolean} True if this set is a strict subset of the given RoaringBitmap32. False if not.
   * @memberof RoaringBitmap32
   */
  public isStrictSubset(other: RoaringBitmap32): boolean

  /**
   * Checks wether this set is equal to another set.
   *
   * Returns false also if the given argument is not a RoaringBitmap32 instance.
   *
   * @param {RoaringBitmap32} other The other set to compare for equality.
   * @returns {boolean} True if the two sets contains the same elements, false if not.
   * @memberof RoaringBitmap32
   */
  public isEqual(other: RoaringBitmap32): boolean

  /**
   * Check whether the two bitmaps intersect.
   *
   * Returns true if there is at least one item in common, false if not.
   *
   * Returns false also if the given argument is not a RoaringBitmap32 instance.
   *
   * @param {RoaringBitmap32} other The other set to compare for intersection.
   * @returns {boolean}  True if the two set intersects, false if not.
   * @memberof RoaringBitmap32
   */
  public intersects(other: RoaringBitmap32): boolean

  /**
   * Computes the size of the intersection between two bitmaps (the number of values in common).
   *
   * Returns -1 if the given argument is not a RoaringBitmap32 instance.
   *
   * @param {RoaringBitmap32} other The other set to compare for intersection.
   * @returns {number} The number of elements in common.
   * @memberof RoaringBitmap32
   */
  public andCardinality(other: RoaringBitmap32): number

  /**
   * Computes the size of the union between two bitmaps.
   *
   * Returns -1 if the given argument is not a RoaringBitmap32 instance.
   *
   * @param {RoaringBitmap32} other The other set to compare for intersection.
   * @returns {number} The number of elements in common.
   * @memberof RoaringBitmap32
   */
  public orCardinality(other: RoaringBitmap32): number

  /**
   * Computes the size of the difference (andnot) between two bitmaps.
   *
   * Returns -1 if the given argument is not a RoaringBitmap32 instance.
   *
   * @param {RoaringBitmap32} other The other set to compare for intersection.
   * @returns {number}  The number of elements in common.
   * @memberof RoaringBitmap32
   */
  public andNotCardinality(other: RoaringBitmap32): number

  /**
   * Computes the size of the symmetric difference (xor) between two bitmaps.
   *
   * Returns -1 if the given argument is not a RoaringBitmap32 instance.
   *
   * @param {RoaringBitmap32} other The other set to compare for intersection.
   * @returns {number} The number of elements in common.
   * @memberof RoaringBitmap32
   */
  public xorCardinality(other: RoaringBitmap32): number

  /**
   * Computes the Jaccard index between two bitmaps.
   * (Also known as the Tanimoto distance or the Jaccard similarity coefficient).
   *
   * See https://en.wikipedia.org/wiki/Jaccard_index
   *
   * The Jaccard index is undefined if both bitmaps are empty.
   *
   * Returns -1 if the given argument is not a RoaringBitmap32 instance.
   *
   * @param {RoaringBitmap32} other The other RoaringBitmap32 to compare.
   * @returns {number} The Jaccard index.
   * @memberof RoaringBitmap32
   */
  public jaccardIndex(other: RoaringBitmap32): number

  /**
   * Remove run-length encoding even when it is more space efficient.
   *
   * Return whether a change was applied.
   *
   * @returns {boolean} True if a change was applied, false if not.
   * @memberof RoaringBitmap32
   */
  public removeRunCompression(): boolean

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
  public runOptimize(): boolean

  /**
   * If needed, reallocate memory to shrink the memory usage.
   *
   * Returns the number of bytes saved.
   *
   * @returns {number} The number of bytes saved.
   * @memberof RoaringBitmap32
   */
  public shrinkToFit(): number

  /**
   *  Returns the number of values in the set that are smaller or equal to the given value.
   *
   * @param {number} maxValue The maximum value
   * @returns {number} Returns the number of values in the set that are smaller or equal to the given value.
   * @memberof RoaringBitmap32
   */
  public rank(maxValue: number): number

  /**
   * If the size of the roaring bitmap is strictly greater than rank,
   * then this function returns the element of given rank.
   *
   * Otherwise, it returns undefined.
   *
   * @param {number} rank The rank, an unsigned 32 bit integer.
   * @returns {(number | undefined)} The element of the given rank or undefined if not found.
   * @memberof RoaringBitmap32
   */
  public select(rank: number): number | undefined

  /**
   * Creates a new Uint32Array and fills it with all the values in the bitmap.
   *
   * The returned array may be very big, up to 4 gigabytes.
   *
   * Use this function only when you know what you are doing.
   *
   * This function is faster than calling new Uint32Array(bitmap);
   *
   * @returns A new Uint32Array instance containing all the items in the set in order.
   * @memberof RoaringBitmap32
   */
  public toUint32Array(): Uint32Array

  /**
   * Creates a new plain JS array and fills it with all the values in the bitmap.
   *
   * The returned array may be very big, use this function only when you know what you are doing.
   *
   * @returns {number[]}  A new plain JS array that contains all the items in the set in order.
   * @memberof RoaringBitmap32
   */
  public toArray(): number[]

  /**
   * Creates a new plain JS Set<number> and fills it with all the values in the bitmap.
   *
   * The returned set may be very big, use this function only when you know what you are doing.
   *
   * @returns {Set<number>} A new plain JS array that contains all the items in the set in order.
   * @memberof RoaringBitmap32
   */
  public toSet(): Set<number>

  /**
   * Returns a plain JS array with all the values in the bitmap.
   *
   * Used by JSON.stringify to serialize this bitmap as an array.
   *
   * @returns {number[]} A new plain JS array that contains all the items in the set in order.
   * @memberof RoaringBitmap32
   */
  public toJSON(): number[]

  /**
   * How many bytes are required to serialize this bitmap.
   *
   * Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
   * The portable version is meant to be compatible with Java and Go versions.
   *
   * @param {boolean} [portable] If false (default), optimized C/C++ format is used. If true, Java and Go portable format is used.
   * @returns {number} How many bytes are required to serialize this bitmap.
   * @memberof RoaringBitmap32
   */
  public getSerializationSizeInBytes(portable?: boolean): number

  /**
   * Serializes the bitmap into a new Buffer.
   *
   * Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
   * The portable version is meant to be compatible with Java and Go versions.
   *
   * @param {boolean} [portable] If false (default), optimized C/C++ format is used. If true, Java and Go portable format is used.
   * @returns {Buffer} A new node Buffer that contains the serialized bitmap.
   * @memberof RoaringBitmap32
   */
  public serialize(portable?: boolean): Buffer

  /**
   * Deserializes the bitmap from an Uint8Array or a Buffer.
   *
   * Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
   * The portable version is meant to be compatible with Java and Go versions.
   *
   * @param {Uint8Array} serialized An Uint8Array or a node Buffer that contains the serialized data.
   * @param {boolean} [portable] If false (default), optimized C/C++ format is used. If true, Java and Go portable format is used.
   * @memberof RoaringBitmap32
   */
  public deserialize(serialized: Uint8Array, portable?: boolean): void

  /**
   * Returns a new RoaringBitmap32 that is a copy of this bitmap, same as new RoaringBitmap32(copy)
   *
   * @returns {RoaringBitmap32} A cloned RoaringBitmap32 instance
   * @memberof RoaringBitmap32
   */
  public clone(): RoaringBitmap32

  /**
   * Returns "RoaringBitmap32(size)".
   *
   * To have a standard string representation of the content as a string, call contentToString() instead.
   *
   * @returns {string} "RoaringBitmap32(size)"
   * @memberof RoaringBitmap32
   */
  public toString(): string

  /**
   * Returns a standard string representation of the content of this RoaringBitmap32 instance. It may return a very long string.
   *
   * Default max length is 32000 characters, everything after maxLength is truncated (ellipsis added).
   *
   * @param {number} [maxLength] Approximate maximum length of the string. Default is 32000. Ellipsis will be added if the string is longer.
   * @returns {string} A string in the format "[1,2,3...]"
   * @memberof RoaringBitmap32
   */
  public contentToString(maxLength?: number): string

  /**
   * Returns an object that contains statistic information about this RoaringBitmap32 instance.
   *
   * @returns {RoaringBitmap32Statistics} An object containing several statistics for the bitmap.
   * @memberof RoaringBitmap32
   */
  public statistics(): RoaringBitmap32Statistics
}

/**
 * Iterator for RoaringBitmap32
 *
 * @export
 * @class RoaringBitmap32Iterator
 * @implements {IterableIterator<number>}
 */
export class RoaringBitmap32Iterator implements IterableIterator<number> {
  // Allows: import RoaringBitmap32Iterator from 'roaring/RoaringBitmap32Iterator'
  private static readonly default: typeof RoaringBitmap32Iterator

  /**
   * Creates a new iterator able to iterate a RoaringBitmap32.
   *
   * @param {RoaringBitmap32} [roaringBitmap32] The roaring bitmap to iterate. If null or undefined, an empty iterator is created.
   * @memberof RoaringBitmap32Iterator
   */
  public constructor(roaringBitmap32?: RoaringBitmap32)

  /**
   * Creates a new iterator able to iterate a RoaringBitmap32.
   *
   * It allocates a small temporary buffer of the given size for speedup.
   *
   * @param {RoaringBitmap32} roaringBitmap32 The roaring bitmap to iterate
   * @param {number} bufferSize Buffer size to allocate, must be an integer greater than 0
   * @memberof RoaringBitmap32Iterator
   */
  public constructor(roaringBitmap32: RoaringBitmap32, bufferSize: number)

  /**
   * Creates a new iterator able to iterate a RoaringBitmap32 using the given temporary buffer.
   *
   * @param {RoaringBitmap32} roaringBitmap32
   * @param {Uint32Array} buffer The roaring bitmap to iterate
   * @memberof RoaringBitmap32Iterator The reusable temporary buffer. Length must be greater than 0.
   */
  public constructor(roaringBitmap32: RoaringBitmap32, buffer: Uint32Array)

  /**
   * Returns this.
   *
   * @returns {this} The same instance.
   * @memberof RoaringBitmap32Iterator
   */
  public [Symbol.iterator](): this

  /**
   * Returns the next element in the iterator.
   *
   * For performance reasons, this function returns always the same instance.
   *
   * @returns {IteratorResult<number>} The next result.
   * @memberof RoaringBitmap32Iterator
   */
  public next(): IteratorResult<number>
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
  containers: number

  /**
   * Number of array containers.
   * @type {number}
   */
  arrayContainers: number

  /**
   * Number of run containers.
   * @type {number}
   */
  runContainers: number

  /**
   * Number of bitmap containers.
   * @type {number}
   */
  bitsetContainers: number

  /**
   * Number of values in array containers.
   * @type {number}
   */
  valuesInArrayContainers: number

  /**
   * Number of values in run containers.
   * @type {number}
   */
  valuesInRunContainers: number

  /**
   * Number of values in  bitmap containers.
   * @type {number}
   */
  valuesInBitsetContainers: number

  /**
   * Number of allocated bytes in array containers.
   * @type {number}
   */
  bytesInArrayContainers: number

  /**
   * Number of allocated bytes in run containers.
   * @type {number}
   */
  bytesInRunContainers: number

  /**
   * Number of allocated bytes in bitmap containers.
   * @type {number}
   */
  bytesInBitsetContainers: number

  /**
   * The maximal value.
   * @type {number}
   */
  maxValue: number

  /**
   * The minimal value.
   * @type {number}
   */
  minValue: number

  /**
   * The sum of all values
   * @type {number}
   */
  sumOfAllValues: number

  /**
   * Total number of values stored in the bitmap
   * @type {number}
   */
  size: number
}

/**
 * Property: The instruction set supported and currently used by the underlying CRoraring library.
 * Possible values are:
 *  - 'AVX2' - Advanced Vector Extensions 2
 *  - 'SSE42' - Streaming SIMD Extensions 4.2
 *  - 'PLAIN' - no special instruction set
 *
 * @export
 * @constant
 * @type {('AVX2' | 'SSE42' | 'PLAIN')}
 * @memberof RoaringModule
 */
export const instructionSet: 'AVX2' | 'SSE42' | 'PLAIN'

/**
 * Property: Indicates wether Streaming SIMD Extensions 4.2 instruction set is supported and currently used by the underlying CRoaring library.
 *
 * @export
 * @constant
 * @type {boolean} True if SSE4.2 is supported, false if not.
 * @memberof RoaringModule
 */
export const SSE42: boolean

/**
 * Property: Indicates wether Advanced Vector Extensions 2 instruction set is supported and currently used by the underlying CRoaring library.
 *
 * @export
 * @constant
 * @type {boolean} True if AVX2 is supported, false if not.
 * @memberof RoaringModule
 */
export const AVX2: boolean

/**
 * Property: The version of the CRoaring libary as a string.
 * Example: "0.2.42"
 *
 * @export
 * @constant
 * @type {string} The version of the CRoaring libary as a string. Example: "0.2.42"
 * @memberof RoaringModule
 */
export const CRoaringVersion: string

/**
 * Property: The version of the roaring npm package as a string.
 * Example: "0.2.2"
 *
 * @export
 * @constant
 * @type {string} The version of the roaring npm package as a string. Example: "0.2.42"
 * @memberof RoaringModule
 */
export const PackageVersion: string

import roaring = require('./')
export default roaring

export type RoaringBitmap32Callback = (error: Error | null, bitmap: RoaringBitmap32 | undefined) => void
export type RoaringBitmap32ArrayCallback = (error: Error | null, bitmap: RoaringBitmap32[] | undefined) => void

// tslint:disable-next-line:no-empty-interface
declare interface Buffer extends Uint8Array {}
