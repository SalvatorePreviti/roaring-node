import RoaringBitmap32Iterator = require('./RoaringBitmap32Iterator')

/**
 * Roaring bitmap that supports 32 bit unsigned integers.
 *
 * See http://roaringbitmap.org/
 *
 */
declare class RoaringBitmap32 implements Iterable<number> {
  // Allows: import RoaringBitmap32 from 'roaring/RoaringBitmap32'
  private static readonly default: typeof RoaringBitmap32

  /**
   * Property. Gets the number of items in the set (cardinality).
   */
  public readonly size: number

  /**
   * Property. True if the bitmap is empty.
   */
  public readonly isEmpty: boolean

  /**
   * Creates an instance of RoaringBitmap32.
   */
  public constructor(values?: Iterable<number>)

  /**
   * Gets a new iterator able to iterate all values in the set in order.
   *
   * @returns A new iterator
   */
  public [Symbol.iterator](): RoaringBitmap32Iterator

  /**
   * Gets the minimum value in the set.
   *
   * @returns The minimum value in the set or 0xFFFFFFFF if empty.
   */
  public minimum(): number

  /**
   * Gets the maximum value in the set.
   *
   * @returns The minimum value in the set or 0 if empty.
   */
  public maximum(): number

  /**
   * Checks wether the given value exists in the set.
   *
   * @param value A 32 bit unsigned integer to search.
   * @returns True if the set contains the given value, false if not.
   */
  public has(value: number): boolean

  /**
   * Adds a single value to the set.
   *
   * @param value The value to add. Must be a valid 32 bit unsigned integer.
   * @returns The same RoaringBitmap32 instance.
   */
  public add(value: number): this

  /**
   * Tries to add a single value to the set.
   * Returns true if the value was added during this call, false if already existing or not a valid unsigned integer 32.
   *
   * @param value The value to add. Must be a valid 32 bit unsigned integer.
   * @returns True if operation was succesfull, false if not.
   */
  public tryAdd(value: number): boolean

  /**
   * Adds multiple values to the set.
   * Faster than calling add() multiple times.
   * It is faster to insert sorted or partially sorted values.
   * This function is optimized if the argument is an instance of RoaringBitmap32 (it performs an OR union).
   *
   * @param values An iterable of values to insert.
   * @returns The same RoaringBitmap32 instance.
   */
  public addMany(values: Iterable<number>): this

  /**
   * Removes a value from the set.
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
   * Faster than calling remove() multiple times.
   * This function is optimized if the argument is an instance of RoaringBitmap32 (it performs an AND NOT operation).
   *
   * @param values An iterable of values to remove.
   * @returns The same RoaringBitmap32 instance.
   */
  public removeMany(values: Iterable<number>): this

  /**
   * Removes all values from the set freeing resources.
   */
  public clear(): void

  /**
   * Performs an union in place ("this = this OR values").
   * Same as addMany.
   *
   * @param values A RoaringBitmap32 instance or an iterable of unsigned 32 bit integers.
   * @returns The same RoaringBitmap32 instance.
   */
  public orInPlace(values: Iterable<number>): this

  /**
   * Performs a AND NOT operation in place ("this = this AND NOT values").
   * Same as removeMany.
   *
   * @param values A RoaringBitmap32 instance or an iterable of unsigned 32 bit integers.
   * @returns The same RoaringBitmap32 instance.
   */
  public andNotInPlace(values: Iterable<number>): this

  /**
   * Performs the intersection (and) between the current bitmap and the provided bitmap,
   * writing the result in the current bitmap.
   * The provided bitmap is not modified.
   *
   * @param values A RoaringBitmap32 instance or an iterable of unsigned 32 bit integers.
   * @returns The same RoaringBitmap32 instance.
   */
  public andInPlace(values: Iterable<number>): this

  /**
   * Performs the symmetric union (xor) between the current bitmap and the provided bitmap,
   * writing the result in the current bitmap.
   * The provided bitmap is not modified.
   *
   * @param values A RoaringBitmap32 instance or an iterable of unsigned 32 bit integers.
   * @returns The same RoaringBitmap32 instance.
   */
  public xorInPlace(values: Iterable<number>): this

  /**
   * Checks wether this set is a subset or the same as the given set.
   * Returns false also if the given argument is not a RoaringBitmap32 instance.
   *
   * @param other The other set.
   * @returns True if this set is a subset of the given RoaringBitmap32. False if not.
   */
  public isSubset(other: RoaringBitmap32): boolean

  /**
   * Checks wether this set is a strict subset of the given set.
   * Returns false if the sets are the same.
   * Returns false also if the given argument is not a RoaringBitmap32 instance.
   *
   * @param other The other set.
   * @returns True if this set is a strict subset of the given RoaringBitmap32. False if not.
   */
  public isStrictSubset(other: RoaringBitmap32): boolean

  /**
   * Checks wether this set is equal to another set.
   * Returns false also if the given argument is not a RoaringBitmap32 instance.
   *
   * @param other The other set to compare for equality.
   * @returns True if the two sets contains the same elements, false if not.
   */
  public isEqual(other: RoaringBitmap32): boolean

  /**
   * Check whether the two bitmaps intersect.
   * Returns true if there is at least one item in common, false if not.
   * Returns false also if the given argument is not a RoaringBitmap32 instance.
   *
   * @param other The other set to compare for intersection.
   * @returns True if the two set intersects, false if not.
   */
  public intersects(other: RoaringBitmap32): boolean

  /**
   * Computes the size of the intersection between two bitmaps (the number of values in common).
   * Returns -1 if the given argument is not a RoaringBitmap32 instance.
   *
   * @param other The other set to compare for intersection.
   * @returns The number of elements in common.
   */
  public andCardinality(other: RoaringBitmap32): number

  /**
   * Computes the size of the union between two bitmaps.
   * Returns -1 if the given argument is not a RoaringBitmap32 instance.
   *
   * @param other The other set to compare for intersection.
   * @returns The number of elements in common.
   */
  public orCardinality(other: RoaringBitmap32): number

  /**
   * Computes the size of the difference (andnot) between two bitmaps.
   * Returns -1 if the given argument is not a RoaringBitmap32 instance.
   *
   * @param other The other set to compare for intersection.
   * @returns The number of elements in common.
   */
  public andNotCardinality(other: RoaringBitmap32): number

  /**
   * Computes the size of the symmetric difference (xor) between two bitmaps.
   * Returns -1 if the given argument is not a RoaringBitmap32 instance.
   *
   * @param other The other set to compare for intersection.
   * @returns The number of elements in common.
   */
  public xorCardinality(other: RoaringBitmap32): number

  /**
   * Computes the Jaccard index between two bitmaps.
   * (Also known as the Tanimoto distance or the Jaccard similarity coefficient).
   * See https://en.wikipedia.org/wiki/Jaccard_index
   *
   * The Jaccard index is undefined if both bitmaps are empty.
   *
   * Returns -1 if the given argument is not a RoaringBitmap32 instance.
   *
   * @param other The other set to compare.
   * @returns The Jaccard index.
   */
  public jaccardIndex(other: RoaringBitmap32): number

  /**
   * Negates in place all the values within a specified interval.
   * Areas outside the range are passed through unchanged.
   * The function does nothing if values are not valid unsigned 32 bit integers.
   *
   * @param rangeStart The start index. Must be a 32 bit integer.
   * @param rangeEnd The end index. Must be a 32 bit integer.
   */
  public flipRange(rangeStart: number, rangeEnd: number): void

  /**
   * Remove run-length encoding even when it is more space efficient.
   * Return whether a change was applied.
   *
   * @returns True if a change was applied, false if not.
   */
  public removeRunCompression(): boolean

  /**
   * Convert array and bitmap containers to run containers when it is more efficient;
   * also convert from run containers when more space efficient.
   * Returns true if the bitmap has at least one run container.
   * Additional savings might be possible by calling shrinkToFit().
   *
   * @returns True if the bitmap has at least one run container.
   */
  public runOptimize(): boolean

  /**
   * If needed, reallocate memory to shrink the memory usage.
   * Returns the number of bytes saved.
   *
   * @returns The number of bytes saved.
   */
  public shrinkToFit(): number

  /**
   *  Returns the number of values in the set that are smaller or equal to the given value.
   *
   * @param maxValue The maximum value
   * @returns Returns the number of values in the set that are smaller or equal to the given value.
   */
  public rank(maxValue: number): number

  /**
   * Creates a new Uint32Array and fills it with all the values in the bitmap.
   * The returned array may be very big, up to 4 gigabytes.
   * Use this function only when you know what you are doing.
   *
   * @returns A new Uint32Array instance containing all the items in the set.
   */
  public toUint32Array(): Uint32Array

  /**
   * How many bytes are required to serialize this bitmap.
   *
   * Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
   * The portable version is meant to be compatible with Java and Go versions.
   *
   * @param portable Optional value. If true, portable format is used. Default is false.
   * @returns How many bytes are required to serialize this bitmap.
   */
  public getSerializationSizeInBytes(portable?: boolean): number

  /**
   * Serializes the bitmap into a new Uint8Array.
   *
   * Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
   * The portable version is meant to be compatible with Java and Go versions.
   *
   * @returns A new Uint8Array that contains the serialized bitmap.
   */
  public serialize(portable?: boolean): Uint8Array

  /**
   * Deserializes the bitmap from an Uint8Array or a Buffer.
   *
   * Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
   * The portable version is meant to be compatible with Java and Go versions.
   */
  public deserialize(serialized: Uint8Array, portable?: boolean): void
}

export = RoaringBitmap32
