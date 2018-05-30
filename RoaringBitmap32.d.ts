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
   * Gets a new iterator able to iterate all values in the set in order.
   *
   * @returns A new iterator
   */
  public values(): RoaringBitmap32Iterator

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
   * @returns The same RoaringBitmap32 instance.
   */
  public remove(value: number): this

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
   * Performs an union in place ("this = this OR values").
   * Same as addMany.
   *
   * @param values A RoaringBitmap32 instance or an iterable of values to insert.
   * @returns The same RoaringBitmap32 instance.
   */
  public orInPlace(values: Iterable<number>): this

  /**
   * Performs a AND NOT operation in place ("this = this AND NOT values").
   * Same as removeMany.
   *
   * @param values A RoaringBitmap32 instance or an iterable of values to insert.
   * @returns The same RoaringBitmap32 instance.
   */
  public andNotInPlace(values: Iterable<number>): this
}

export = RoaringBitmap32
