import RoaringBitmap32 from '../../RoaringBitmap32'

describe('RoaringBitmap32 comparisons', () => {
  describe('isSubset', () => {
    it('returns false for invalid values', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.isSubset(null as any)).toBe(false)
      expect(bitmap.isSubset(undefined as any)).toBe(false)
      expect(bitmap.isSubset(123 as any)).toBe(false)
      expect(bitmap.isSubset([123] as any)).toBe(false)
    })

    it('returns true with itself', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.isSubset(bitmap)).toBe(true)
    })

    it('returns true with another empty set', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32()
      expect(a.isSubset(b)).toBe(true)
    })

    it('returns true with another non empty set', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32([1, 2, 3])
      expect(a.isSubset(b)).toBe(true)
    })

    it('returns true for bitmap1 < bitmap2', () => {
      const a = new RoaringBitmap32([1, 2])
      const b = new RoaringBitmap32([1, 2, 3])
      expect(a.isSubset(b)).toBe(true)
    })

    it('returns true for bitmap1 == bitmap2', () => {
      const a = new RoaringBitmap32([1, 2, 3])
      const b = new RoaringBitmap32([1, 2, 3])
      expect(a.isSubset(b)).toBe(true)
    })

    it('returns true for bitmap1 === bitmap1', () => {
      const bitmap = new RoaringBitmap32([1, 2, 3])
      expect(bitmap.isSubset(bitmap)).toBe(true)
    })

    it('returns false for bitmap1 > bitmap1', () => {
      const a = new RoaringBitmap32([1, 3, 2])
      const b = new RoaringBitmap32([1, 2])
      expect(a.isSubset(b)).toBe(false)
    })
  })

  describe('isStrictSubset', () => {
    it('returns false for invalid values', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.isStrictSubset(null as any)).toBe(false)
      expect(bitmap.isStrictSubset(undefined as any)).toBe(false)
      expect(bitmap.isStrictSubset(123 as any)).toBe(false)
      expect(bitmap.isStrictSubset([123] as any)).toBe(false)
    })

    it('returns false with itself (empty)', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.isStrictSubset(bitmap)).toBe(false)
    })

    it('returns false with itself (not empty)', () => {
      const bitmap = new RoaringBitmap32([1, 2, 3])
      expect(bitmap.isStrictSubset(bitmap)).toBe(false)
    })

    it('returns false with another empty set', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32()
      expect(a.isStrictSubset(b)).toBe(false)
    })

    it('returns true with another non empty set', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32([1, 2, 3])
      expect(a.isStrictSubset(b)).toBe(true)
    })

    it('returns true for bitmap1 < bitmap2', () => {
      const a = new RoaringBitmap32([1, 2])
      const b = new RoaringBitmap32([1, 2, 3])
      expect(a.isStrictSubset(b)).toBe(true)
    })

    it('returns false for bitmap1 == bitmap2', () => {
      const a = new RoaringBitmap32([1, 2, 3])
      const b = new RoaringBitmap32([1, 2, 3])
      expect(a.isStrictSubset(b)).toBe(false)
    })

    it('returns false for bitmap1 === bitmap1', () => {
      const a = new RoaringBitmap32([1, 2, 3])
      expect(a.isStrictSubset(a)).toBe(false)
    })

    it('returns false for bitmap1 > bitmap2', () => {
      const a = new RoaringBitmap32([1, 2, 3])
      const b = new RoaringBitmap32([1, 2])
      expect(a.isStrictSubset(b)).toBe(false)
    })
  })

  describe('isEqual', () => {
    it('returns false for invalid values', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.isEqual(null as any)).toBe(false)
      expect(bitmap.isEqual(undefined as any)).toBe(false)
      expect(bitmap.isEqual(123 as any)).toBe(false)
      expect(bitmap.isEqual([123] as any)).toBe(false)
    })

    it('returns true with itself', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.isEqual(bitmap)).toBe(true)
    })

    it('returns true with another empty set', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32()
      expect(a.isEqual(b)).toBe(true)
    })

    it('returns false with another non empty set', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32([1, 2, 3])
      expect(a.isEqual(b)).toBe(false)
    })

    it('returns true if two bitmaps are the same', () => {
      const a = new RoaringBitmap32([3, 2, 1])
      expect(a.isEqual(a)).toBe(true)
    })

    it('returns true if two bitmaps are equal', () => {
      const a = new RoaringBitmap32([3, 2, 1])
      const b = new RoaringBitmap32([1, 2, 3])
      expect(a.isEqual(b)).toBe(true)
    })

    it('returns false if two bitmaps are not equal', () => {
      const a = new RoaringBitmap32([3, 2, 4])
      const b = new RoaringBitmap32([1, 2, 3])
      expect(a.isEqual(b)).toBe(false)
    })
  })

  describe('intersects', () => {
    it('returns false for invalid values', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.intersects(null as any)).toBe(false)
      expect(bitmap.intersects(undefined as any)).toBe(false)
      expect(bitmap.intersects(123 as any)).toBe(false)
      expect(bitmap.intersects([123] as any)).toBe(false)
    })

    it('returns false with itself (empty)', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.intersects(bitmap)).toBe(false)
    })

    it('returns false with another empty set', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32()
      expect(a.intersects(b)).toBe(false)
    })

    it('returns false with another non empty set', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32([1, 2, 3])
      expect(a.intersects(b)).toBe(false)
    })

    it('returns true if bitmap1 < bitmap2', () => {
      const a = new RoaringBitmap32([1, 2])
      const b = new RoaringBitmap32([1, 2, 3])
      expect(a.intersects(b)).toBe(true)
    })

    it('returns true if bitmap1 > bitmap2', () => {
      const a = new RoaringBitmap32([1, 2, 3])
      const b = new RoaringBitmap32([1, 2])
      expect(a.intersects(b)).toBe(true)
    })

    it('returns true if bitmap1 has one value in common with bitmap2', () => {
      const a = new RoaringBitmap32([1, 2, 3])
      const b = new RoaringBitmap32([3, 5, 2])
      expect(a.intersects(b)).toBe(true)
    })

    it('returns true if bitmap1 has more than one value in common with bitmap2', () => {
      const a = new RoaringBitmap32([1, 2, 3, 100, 101, 102])
      const b = new RoaringBitmap32([3, 100, 101, 102, 5])
      expect(a.intersects(b)).toBe(true)
    })

    it('returns false if bitmap1 has no value in common with bitmap2', () => {
      const a = new RoaringBitmap32([1, 2, 3])
      const b = new RoaringBitmap32([0, 100, 200])
      expect(a.intersects(b)).toBe(false)
    })
  })

  describe('andCardinality', () => {
    it('returns -1 for invalid values', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.andCardinality(null as any)).toBe(-1)
      expect(bitmap.andCardinality(undefined as any)).toBe(-1)
      expect(bitmap.andCardinality(123 as any)).toBe(-1)
      expect(bitmap.andCardinality([123] as any)).toBe(-1)
    })

    it('returns 0 with itself (empty)', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.andCardinality(bitmap)).toBe(0)
    })

    it('returns size with itself (not empty)', () => {
      const bitmap = new RoaringBitmap32([3, 4, 5, 6])
      expect(bitmap.andCardinality(bitmap)).toBe(4)
    })

    it('returns 0 between an empty set and another empty set', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32()
      expect(a.andCardinality(b)).toBe(0)
    })

    it('returns 0 between a non empty set and an empty set', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32([1, 2, 3])
      expect(a.andCardinality(b)).toBe(0)
    })
  })

  describe('orCardinality', () => {
    it('returns -1 for invalid values', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.orCardinality(null as any)).toBe(-1)
      expect(bitmap.orCardinality(undefined as any)).toBe(-1)
      expect(bitmap.orCardinality(123 as any)).toBe(-1)
      expect(bitmap.orCardinality([123] as any)).toBe(-1)
    })

    it('returns 0 with itself (empty)', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.orCardinality(bitmap)).toBe(0)
    })

    it('returns 0 with two empty sets', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32()
      expect(a.orCardinality(b)).toBe(0)
    })

    it('returns a.size with a non empty, b empty', () => {
      const a = new RoaringBitmap32([1, 2, 3])
      const b = new RoaringBitmap32()
      expect(a.orCardinality(b)).toBe(3)
    })

    it('returns b.size with a empty, b not empty', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32([1, 2, 3])
      expect(a.orCardinality(b)).toBe(3)
    })

    it('returns the union count', () => {
      const a = new RoaringBitmap32([1, 4, 5])
      const b = new RoaringBitmap32([1, 2, 3])
      expect(a.orCardinality(b)).toBe(5)
    })
  })

  describe('andNotCardinality', () => {
    it('returns -1 for invalid values', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.andNotCardinality(null as any)).toBe(-1)
      expect(bitmap.andNotCardinality(undefined as any)).toBe(-1)
      expect(bitmap.andNotCardinality(123 as any)).toBe(-1)
      expect(bitmap.andNotCardinality([123] as any)).toBe(-1)
    })

    it('returns 0 with itself (empty)', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.andNotCardinality(bitmap)).toBe(0)
    })

    it('returns 0 with two empty sets', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32()
      expect(a.andNotCardinality(b)).toBe(0)
    })

    it('returns 0 with itself (not empty)', () => {
      const bitmap = new RoaringBitmap32([4, 70, 90])
      expect(bitmap.andNotCardinality(bitmap)).toBe(0)
    })

    it('returns 0 with two equal sets', () => {
      const a = new RoaringBitmap32([1, 2, 3])
      const b = new RoaringBitmap32([1, 2, 3])
      expect(a.andNotCardinality(b)).toBe(0)
    })

    it('returns the difference count', () => {
      const a = new RoaringBitmap32([1, 2, 3])
      const b = new RoaringBitmap32([1, 4, 5])
      expect(a.andNotCardinality(b)).toBe(2)
    })
  })

  describe('xorCardinality', () => {
    it('returns -1 for invalid values', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.xorCardinality(null as any)).toBe(-1)
      expect(bitmap.xorCardinality(undefined as any)).toBe(-1)
      expect(bitmap.xorCardinality(123 as any)).toBe(-1)
      expect(bitmap.xorCardinality([123] as any)).toBe(-1)
    })

    it('returns 0 with itself (empty)', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.xorCardinality(bitmap)).toBe(0)
    })

    it('returns 0 with itself (not empty)', () => {
      const bitmap = new RoaringBitmap32([1, 3, 4])
      expect(bitmap.xorCardinality(bitmap)).toBe(0)
    })

    it('returns 0 with two empty sets', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32()
      expect(a.xorCardinality(b)).toBe(0)
    })

    it('returns b.size with a empty, b not empty', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32([1, 2, 3])
      expect(a.xorCardinality(b)).toBe(3)
    })

    it('returns a.size with a not empty, b empty', () => {
      const a = new RoaringBitmap32([1, 2, 3])
      const b = new RoaringBitmap32()
      expect(a.xorCardinality(b)).toBe(3)
    })

    it('returns the xor count', () => {
      const a = new RoaringBitmap32([1, 2, 3])
      const b = new RoaringBitmap32([1, 4, 6])
      expect(a.xorCardinality(b)).toBe(4)
    })
  })
})
