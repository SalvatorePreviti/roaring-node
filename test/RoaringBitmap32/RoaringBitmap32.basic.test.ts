import RoaringBitmap32 = require('../../RoaringBitmap32')

describe('RoaringBitmap32 basic', () => {
  describe('constructor', () => {
    it('is a class', () => {
      expect(typeof RoaringBitmap32).toBe('function')
    })

    it('creates an empty bitmap', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.size).toBe(0)
      expect(bitmap.isEmpty).toBe(true)
    })
  })
})
