import RoaringBitmap32 = require('./RoaringBitmap32')
import RoaringBitmap32Iterator = require('./RoaringBitmap32Iterator')

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
   * Property. Gets the version of the CRoaring libary as a string.
   * Example: "0.2.42"
   */
  public readonly version: string

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
