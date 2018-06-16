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
   * Is faster to pass a Uint32Array instance instead of an array or an iterable.
   * Is optimized if the given argument is a RoaringBitmap32 (performs a fast copy).
   */
  public constructor(values?: Iterable<number>)

  /**
   * Creates an instance of RoaringBitmap32 from the given Iterable.
   * Is faster to pass a Uint32Array instance instead of an array or an iterable.
   * Is optimized if the given argument is a RoaringBitmap32 (performs a fast copy).
   *
   * @param values The values to set.
   * @returns A new RoaringBitmap32 instance filled with the given values.
   */
  public static from(values: Iterable<number>): RoaringBitmap32

  /**
   * Deserializes the bitmap from an Uint8Array or a Buffer.
   * Returns a new RoaringBitmap32 instance.
   *
   * Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
   * The portable version is meant to be compatible with Java and Go versions.
   */
  public static deserialize(serialized: Uint8Array, portable?: boolean): RoaringBitmap32

  /**
   * Swaps the content of two RoaringBitmap32 instances.
   *
   * @param a First RoaringBitmap32 instance to swap
   * @param b Second RoaringBitmap32 instance to swap
   */
  public static swap(a: RoaringBitmap32, b: RoaringBitmap32): void

  /**
   * Returns a new RoaringBitmap32 with the intersection (and) between the given two bitmaps.
   * The provided bitmaps are not modified.
   *
   * @param a The first RoaringBitmap32 instance to and.
   * @param b The second RoaringBitmap32 instance to and.
   * @returns A new bitmap, a AND b
   */
  public static and(a: RoaringBitmap32, b: RoaringBitmap32): RoaringBitmap32

  /**
   * Returns a new RoaringBitmap32 with the union (or) of the two given bitmaps.
   * The provided bitmaps are not modified.
   *
   * @param a The first RoaringBitmap32 instance to or.
   * @param b The second RoaringBitmap32 instance to or.
   * @returns A new bitmap, a OR b
   */
  public static or(a: RoaringBitmap32, b: RoaringBitmap32): RoaringBitmap32

  /**
   * Returns a new RoaringBitmap32 with the symmetric union (xor) between the two given bitmaps.
   * The provided bitmaps are not modified.
   *
   * @param a The first RoaringBitmap32 instance to xor.
   * @param b The second RoaringBitmap32 instance to xor.
   * @returns A new bitmap, a XOR b
   */
  public static xor(a: RoaringBitmap32, b: RoaringBitmap32): RoaringBitmap32

  /**
   * Returns a new RoaringBitmap32 with the difference (and not) between the two given bitmaps.
   * The provided bitmaps are not modified.
   *
   * @param a The first RoaringBitmap32 instance.
   * @param b The second RoaringBitmap32 instance.
   * @returns A new bitmap, a AND NOT b
   */
  public static andNot(a: RoaringBitmap32, b: RoaringBitmap32): RoaringBitmap32

  /**
   * Performs a union between all the given RoaringBitmap32 instances.
   * This function is faster than calling or multiple times.
   *
   * @param values An array of RoaringBitmap32 instances to or together.
   * @returns A new RoaringBitmap32 that contains the union of all the given bitmaps.
   */
  public static orMany(values: RoaringBitmap32[]): RoaringBitmap32

  /**
   * Performs a union between all the given RoaringBitmap32 instances.
   * This function is faster than calling or multiple times.
   *
   * @param values The RoaringBitmap32 instances to or together.
   * @returns A new RoaringBitmap32 that contains the union of all the given bitmaps.
   */
  public static orMany(...values: RoaringBitmap32[]): RoaringBitmap32

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
   *
   * @param rangeStart The start index. Must be an non-negative number less or equal to 4294967296.
   * @param rangeEnd The end index. Must be a non-negative number less or equal to 4294967297.
   */
  public hasRange(rangeStart: number, rangeEnd: number): boolean

  /**
   * Overwrite the content of this bitmap copying it from an Iterable or another RoaringBitmap32.
   * This function is optimized for copying RoaringBitmap32 instances.
   *
   * @param values The new values or a RoaringBitmap32 instance.
   */
  public copyFrom(values: Iterable<number>): void

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
   * Is faster to use Uint32Array instead of arrays or iterables.
   * Is optimized if the argument is an instance of RoaringBitmap32 (it performs an OR union).
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
   * Is faster to use Uint32Array instead of arrays or iterables.
   * This function is optimized if the argument is an instance of RoaringBitmap32 (it performs an AND NOT operation).
   *
   * @param values An iterable of values to remove.
   * @returns The same RoaringBitmap32 instance.
   */
  public removeMany(values: Iterable<number>): this

  /**
   * Negates (in place) the roaring bitmap within a specified interval: [rangeStart, rangeEnd).
   * First element is included, last element is excluded.
   * The number of negated values is rangeEnd - rangeStart.
   * Areas outside the range are passed through unchanged.
   *
   * @param rangeStart The start index. Trimmed to 0.
   * @param rangeEnd The end index. Trimmed to 4294967297.
   */
  public flipRange(rangeStart: number, rangeEnd: number): void

  /**
   * Adds all the values in the interval: [rangeStart, rangeEnd).
   * First element is included, last element is excluded.
   * The number of added values is rangeEnd - rangeStart.
   * Areas outside the range are passed through unchanged.
   *
   * @param rangeStart The start index. Trimmed to 0.
   * @param rangeEnd The end index. Trimmed to 4294967297.
   */
  public addRange(rangeStart: number, rangeEnd: number): void

  /**
   * Removes all values from the set.
   * It frees resources, if needed you can use clear to free some memory before the garbage collector disposes this instance.
   */
  public clear(): void

  /**
   * Performs an union in place ("this = this OR values").
   * Is faster to use Uint32Array instead of arrays or iterables.
   * This function is optimized if the argument is an instance of RoaringBitmap32.
   * Same as addMany.
   *
   * @param values A RoaringBitmap32 instance or an iterable of unsigned 32 bit integers.
   * @returns The same RoaringBitmap32 instance.
   */
  public orInPlace(values: Iterable<number>): this

  /**
   * Performs a AND NOT operation in place ("this = this AND NOT values").
   * Is faster to use Uint32Array instead of arrays or iterables.
   * This function is optimized if the argument is an instance of RoaringBitmap32.
   * Same as removeMany.
   *
   * @param values A RoaringBitmap32 instance or an iterable of unsigned 32 bit integers.
   * @returns The same RoaringBitmap32 instance.
   */
  public andNotInPlace(values: Iterable<number>): this

  /**
   * Performs the intersection (and) between the current bitmap and the provided bitmap,
   * writing the result in the current bitmap.
   * Is faster to use Uint32Array instead of arrays or iterables.
   * This function is optimized if the argument is an instance of RoaringBitmap32.
   * The provided bitmap is not modified.
   *
   * @param values A RoaringBitmap32 instance or an iterable of unsigned 32 bit integers.
   * @returns The same RoaringBitmap32 instance.
   */
  public andInPlace(values: Iterable<number>): this

  /**
   * Performs the symmetric union (xor) between the current bitmap and the provided bitmap,
   * writing the result in the current bitmap.
   * Is faster to use Uint32Array instead of arrays or iterables.
   * This function is optimized if the argument is an instance of RoaringBitmap32.
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
   * If the size of the roaring bitmap is strictly greater than rank,
   * then this function returns the element of given rank.
   * Otherwise, it returns undefined.
   *
   * @param rank The rank, an unsigned 32 bit integer.
   * @returns The element of the given rank or undefined if not found.
   */
  public select(rank: number): number | undefined

  /**
   * Creates a new Uint32Array and fills it with all the values in the bitmap.
   * The returned array may be very big, up to 4 gigabytes.
   * Use this function only when you know what you are doing.
   *
   * @returns A new Uint32Array instance containing all the items in the set in order.
   */
  public toUint32Array(): Uint32Array

  /**
   * Creates a new plain JS array and fills it with all the values in the bitmap.
   * The returned array may be very big, use this function only when you know what you are doing.
   *
   * @returns A new plain JS array that contains all the items in the set in order.
   */
  public toArray(): number[]

  /**
   * Creates a new plain JS Set<number> and fills it with all the values in the bitmap.
   * The returned set may be very big, use this function only when you know what you are doing.
   *
   * @returns A new plain JS array that contains all the items in the set in order.
   */
  public toSet(): Set<number>

  /**
   * Returns a plain JS array with all the values in the bitmap.
   * Used by JSON.stringify to serialize this bitmap.
   *
   * @returns A new plain JS array that contains all the items in the set in order.
   */
  public toJSON(): number[]

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
   * Serializes the bitmap into a new Buffer.
   *
   * Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
   * The portable version is meant to be compatible with Java and Go versions.
   *
   * @returns A new Buffer that contains the serialized bitmap.
   */
  public serialize(portable?: boolean): Buffer

  /**
   * Deserializes the bitmap from an Uint8Array or a Buffer.
   *
   * Setting the portable flag to false enable a custom format that can save space compared to the portable format (e.g., for very sparse bitmaps).
   * The portable version is meant to be compatible with Java and Go versions.
   */
  public deserialize(serialized: Uint8Array, portable?: boolean): void

  /**
   * Returns a new RoaringBitmap32 that is a copy of this bitmap.
   * Same as new RoaringBitmap32(copy)
   */
  public clone(): RoaringBitmap32

  /**
   * Returns "RoaringBitmap32(size)".
   * To have a standard string representation of the content as a string, call contentToString() instead.
   */
  public toString(): string

  /**
   * Returns a standard string representation of the content of this RoaringBitmap32 instance. It may return a very long string.
   * Default max length is 32000 characters, everything after maxLength is truncated (ellipsis added).
   *
   * @param maxLength Approximate maximum length of the string. Ellipsis will be added if the string is longer.
   * @returns A string in the format "[1,2,3...]"
   */
  public contentToString(maxLength?: number): string

  /**
   * Returns an object that contains several statistinc information about this RoaringBitmap32 instance.
   */
  public statistics(): {
    /**
     * Number of containers.
     */
    containers: number

    /*
     * Number of array containers.
     */
    arrayContainers: number

    /*
     * Number of run containers.
     */
    runContainers: number

    /*
     * Number of bitmap containers.
     */
    bitsetContainers: number

    /*
     * Number of values in array containers.
     */
    valuesInArrayContainers: number

    /*
     * Number of values in run containers.
     */
    valuesInRunContainers: number

    /*
     * Number of values in  bitmap containers.
     */
    valuesInBitsetContainers: number

    /*
     * Number of allocated bytes in array containers.
     */
    bytesInArrayContainers: number

    /*
     * Number of allocated bytes in run containers.
     */
    bytesInRunContainers: number

    /*
     * Number of allocated bytes in bitmap containers.
     */
    bytesInBitsetContainers: number

    /*
     * The maximal value.
     */
    maxValue: number

    /* 
     * The minimal value.
     */
    minValue: number

    /*
     * The sum of all values
     */
    sumOfAllValues: number

    /*
     * Total number of values stored in the bitmap
     */
    size: number
  }
}

/**
 * Roaring bitmap that supports 32 bit unsigned integers.
 *
 * See http://roaringbitmap.org/
 *
 */
declare class RoaringBitmap32Iterator implements IterableIterator<number> {
  // Allows: import RoaringBitmap32Iterator from 'roaring/RoaringBitmap32Iterator'
  private static readonly default: typeof RoaringBitmap32Iterator

  /**
   * Creates a new iterator able to iterate a RoaringBitmap32.
   * @param roaringBitmap32 The roaring bitmap to iterate
   */
  public constructor(roaringBitmap32?: RoaringBitmap32)

  /**
   * Returns this.
   *
   * @returns this
   */
  public [Symbol.iterator](): this

  /**
   * Returns the next element in the iterator.
   *
   * @returns The next result.
   */
  public next(): IteratorResult<number>
}

/**
 * Roaring main module
 */
declare class RoaringModule {
  /**
   * 32 bit roaring bitmap class
   */
  public readonly RoaringBitmap32: typeof RoaringBitmap32

  /**
   * Iterator class for RoaringBitmap32
   */
  public readonly RoaringBitmap32Iterator: typeof RoaringBitmap32Iterator

  /**
   * Property: The version of the CRoaring libary as a string.
   * Example: "0.2.42"
   */
  public readonly CRoaringVersion: string

  /**
   * Property: Indicates wether Streaming SIMD Extensions 4.2 instruction set is supported and currently used by the underlying CRoaring library.
   */
  public readonly SSE42: boolean

  /**
   * Property: Indicates wether Advanced Vector Extensions 2 instruction set is supported and currently used by the underlying CRoaring library.
   */
  public readonly AVX2: boolean

  /**
   * Property: The instruction set supported and currently used by the underlying CRoraring library.
   * Possible values are:
   *  - 'AVX2' - Advanced Vector Extensions 2
   *  - 'SSE42' - Streaming SIMD Extensions 4.2
   *  - 'PLAIN' - no special instruction set
   */
  public readonly instructionSet: 'AVX2' | 'SSE42' | 'PLAIN'

  /**
   * Property: The version of the roaring npm package as a string.
   * Example: "0.2.2"
   */
  public readonly PackageVersion: string

  // Allows: import roaring from 'roaring'
  private readonly default: RoaringModule
}

/**
 * roaring module
 */
declare const roaring: RoaringModule

/**
 * roaring module
 */
export = roaring
