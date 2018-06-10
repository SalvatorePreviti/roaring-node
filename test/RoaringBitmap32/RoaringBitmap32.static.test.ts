import RoaringBitmap32 from '../../RoaringBitmap32'

describe('RoaringBitmap32 static', () => {
  describe('static and', () => {
    it('returns empty with empty bitmaps', () => {
      const bitmap = RoaringBitmap32.and(new RoaringBitmap32(), new RoaringBitmap32())
      expect(bitmap).toBeInstanceOf(RoaringBitmap32)
      expect(bitmap.isEmpty).toBe(true)
    })

    it('returns empty with an non empty bitmap', () => {
      const bitmap = RoaringBitmap32.and(new RoaringBitmap32(), new RoaringBitmap32([1, 2, 3]))
      expect(bitmap).toBeInstanceOf(RoaringBitmap32)
      expect(bitmap.isEmpty).toBe(true)
    })

    it('ands two bitmaps', () => {
      const bitmap = RoaringBitmap32.and(new RoaringBitmap32([3, 1, 2]), new RoaringBitmap32([4, 2]))
      expect(bitmap).toBeInstanceOf(RoaringBitmap32)
      expect(bitmap.toArray()).toEqual([2])
    })
  })

  describe('static or', () => {
    it('returns empty with empty bitmaps', () => {
      const bitmap = RoaringBitmap32.or(new RoaringBitmap32(), new RoaringBitmap32())
      expect(bitmap).toBeInstanceOf(RoaringBitmap32)
      expect(bitmap.isEmpty).toBe(true)
    })

    it('performs or with empty and non empty', () => {
      const c = RoaringBitmap32.or(new RoaringBitmap32(), new RoaringBitmap32([1, 3, 2]))
      expect(c.toArray()).toEqual([1, 2, 3])
    })

    it('ors two bitmaps', () => {
      const bitmap = RoaringBitmap32.or(new RoaringBitmap32([3, 1, 2]), new RoaringBitmap32([4, 2]))
      expect(bitmap.toArray()).toEqual([1, 2, 3, 4])
    })
  })

  describe('static xor', () => {
    it('returns empty with empty bitmaps', () => {
      const c = RoaringBitmap32.xor(new RoaringBitmap32(), new RoaringBitmap32())
      expect(c.isEmpty).toBe(true)
    })

    it('performs or with empty and non empty', () => {
      const bitmap = RoaringBitmap32.xor(new RoaringBitmap32(), new RoaringBitmap32([1, 2, 3]))
      expect(bitmap.toArray()).toEqual([1, 2, 3])
    })

    it('xors two bitmaps', () => {
      const bitmap = RoaringBitmap32.xor(new RoaringBitmap32([3, 1, 2]), new RoaringBitmap32([4, 2]))
      expect(bitmap.toArray()).toEqual([1, 3, 4])
    })
  })

  describe('static andNot', () => {
    it('returns empty with empty bitmaps', () => {
      const c = RoaringBitmap32.andNot(new RoaringBitmap32(), new RoaringBitmap32())
      expect(c.isEmpty).toBe(true)
    })

    it('returns empty  with an non empty array', () => {
      const c = RoaringBitmap32.andNot(new RoaringBitmap32(), new RoaringBitmap32([1, 2, 3]))
      expect(c.isEmpty).toBe(true)
    })

    it('andnots two bitmaps', () => {
      const bitmap = RoaringBitmap32.andNot(new RoaringBitmap32([3, 1, 2]), new RoaringBitmap32([4, 2]))
      expect(bitmap.toArray()).toEqual([1, 3])
    })
  })

  describe('static orMany', () => {
    it('orManys multiple bitmaps (spread arguments)', () => {
      const bitmap = RoaringBitmap32.orMany(new RoaringBitmap32([3, 1, 2]), new RoaringBitmap32([1, 2, 4]), new RoaringBitmap32([1, 2, 6]))
      expect(bitmap.toArray()).toEqual([1, 2, 3, 4, 6])
    })

    it('orManys multiple bitmaps (array)', () => {
      const bitmap = RoaringBitmap32.orMany([
        new RoaringBitmap32([3, 1, 2]),
        new RoaringBitmap32([1, 2, 4]),
        new RoaringBitmap32([1, 2, 6])
      ])
      expect(bitmap.toArray()).toEqual([1, 2, 3, 4, 6])
    })
  })
})
