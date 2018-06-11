import RoaringBitmap32 = require('../../RoaringBitmap32')

describe('RoaringBitmap32 empty', () => {
  describe('removeRunCompression', () => {
    it('does nothing', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.removeRunCompression()).toBe(false)
    })
  })

  describe('runOptimize', () => {
    it('does nothing', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.runOptimize()).toBe(false)
    })
  })

  describe('shrinkToFit', () => {
    it('shrinks some memory', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.shrinkToFit()).toBe(44)
      expect(bitmap.shrinkToFit()).toBe(0)
    })
  })

  describe('getSerializationSizeInBytes', () => {
    it('returns standard value for empty bitmap (non portable)', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.getSerializationSizeInBytes()).toBe(5)
      expect(bitmap.getSerializationSizeInBytes(false)).toBe(5)
    })

    it('returns standard value for empty bitmap (portable)', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.getSerializationSizeInBytes(true)).toBe(8)
    })
  })
})
