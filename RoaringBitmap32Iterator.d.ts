import RoaringBitmap32 = require('./RoaringBitmap32')

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

export = RoaringBitmap32Iterator
