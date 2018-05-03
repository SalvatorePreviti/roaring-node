/**
 * Roaring bitmap that supports 32 bit unsigned integers.
 *
 * See http://roaringbitmap.org/
 *
 * @class RoaringBitmap32
 */
declare class RoaringBitmap32 {
  // Allows: import RoaringBitmap32 from 'roaring/RoaringBitmap32'
  private static readonly default: typeof RoaringBitmap32

  public readonly size: number
  public readonly isEmpty: boolean

  public constructor(values?: Iterable<number>)

  public add(value: number): this
  public addMany(values: Iterable<number>): this
}

// Allows typescript syntax: import RoaringBitmap32 = require('roaring/RoaringBitmap32')
export = RoaringBitmap32
