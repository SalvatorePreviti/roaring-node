import RoaringBitmap32 from './RoaringBitmap32'

declare class RoaringModule {
  /**
   * 32 bit roaring bitmap
   */
  public readonly RoaringBitmap32: typeof RoaringBitmap32

  // Allows: import roaring from 'roaring'
  private readonly default: typeof roaring
}

/**
 * roaring module
 */
declare const roaring: RoaringModule

/**
 * roaring module
 */
export = roaring
