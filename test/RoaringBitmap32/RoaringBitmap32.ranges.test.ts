import RoaringBitmap32 from '../../RoaringBitmap32'
import RoaringBitmap32Iterator from '../../RoaringBitmap32Iterator'

describe('RoaringBitmap32 ranges', () => {
  describe('fromRange', () => {
    it('returns an empty bitmap with invalid values', () => {
      expect(RoaringBitmap32.fromRange(-1, -1).isEmpty).toBe(true)
      expect(RoaringBitmap32.fromRange(89999999999, 99999999999).isEmpty).toBe(true)
      expect(RoaringBitmap32.fromRange(0, 0).isEmpty).toBe(true)
      expect(RoaringBitmap32.fromRange(20, 20).isEmpty).toBe(true)
      expect(RoaringBitmap32.fromRange(null as any, 20).isEmpty).toBe(true)
      expect(RoaringBitmap32.fromRange(undefined as any, 20).isEmpty).toBe(true)
    })

    it('works with range 0, 6', () => {
      const bitmap = RoaringBitmap32.fromRange(0, 6)
      expect(bitmap.size).toBe(6)
      expect(bitmap.toArray()).toEqual([0, 1, 2, 3, 4, 5])
    })

    it('works with range 10, 16', () => {
      const bitmap = RoaringBitmap32.fromRange(10, 16)
      expect(bitmap.size).toBe(6)
      expect(bitmap.toArray()).toEqual([10, 11, 12, 13, 14, 15])
    })

    it('works with range 100, 70000', () => {
      const bitmap = RoaringBitmap32.fromRange(100, 70000)
      expect(bitmap.size).toBe(69900)
    })

    it('works with step 2', () => {
      const bitmap = RoaringBitmap32.fromRange(0, 20, 2)
      expect(bitmap.toArray()).toEqual([0, 2, 4, 6, 8, 10, 12, 14, 16, 18])
    })

    it('works with step 5', () => {
      const bitmap = RoaringBitmap32.fromRange(0, 53, 5)
      expect(bitmap.toArray()).toEqual([0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50])
    })
  })

  describe('hasRange', () => {
    it('returns false for invalid values', () => {
      const bitmap = new RoaringBitmap32([1, 2, 3, 4, 5])
      expect(bitmap.hasRange(undefined as any, 6)).toBe(false)
      expect(bitmap.hasRange(null as any, 6)).toBe(false)
      expect(bitmap.hasRange('0' as any, 6)).toBe(false)
      expect(bitmap.hasRange(0, [8] as any)).toBe(false)
      expect(bitmap.hasRange(NaN, 4)).toBe(false)
      expect(bitmap.hasRange(4, 4)).toBe(false)
      expect(bitmap.hasRange(-2, -4)).toBe(false)
      expect(bitmap.hasRange(-4, -2)).toBe(false)
      expect(bitmap.hasRange(4, 2)).toBe(false)
      expect(bitmap.hasRange(4, -2)).toBe(false)
    })

    it('returns false for an empty bitmap', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.hasRange(0, 3)).toBe(false)
      expect(bitmap.hasRange(0, 0xffffffff)).toBe(false)
      expect(bitmap.hasRange(0, 0xffffffff + 1)).toBe(false)
      expect(bitmap.hasRange(0xfffffffe, 0xffffffff + 1)).toBe(false)
      expect(bitmap.hasRange(0xffffffff, 0xffffffff + 1)).toBe(false)
    })

    it('returns false for areas that contains no values', () => {
      const bitmap = new RoaringBitmap32([100, 101, 102, 103, 105])
      expect(bitmap.hasRange(99, 101)).toBe(false)
      expect(bitmap.hasRange(105, 107)).toBe(false)
      expect(bitmap.hasRange(105, 109)).toBe(false)
      expect(bitmap.hasRange(100, 106)).toBe(false)
    })

    it('works with simple values', () => {
      const bitmap = new RoaringBitmap32([100, 101, 102, 103, 105])
      expect(bitmap.hasRange(100, 102)).toBe(true)
      expect(bitmap.hasRange(100, 103)).toBe(true)
      expect(bitmap.hasRange(100, 104)).toBe(true)
      expect(bitmap.hasRange(100, 105)).toBe(false)
    })

    it('accepts floating point values', () => {
      const bitmap = new RoaringBitmap32([50, 51, 52, 53, 55])

      expect(bitmap.hasRange(50, 52)).toBe(true)
      expect(bitmap.hasRange(50, 53)).toBe(true)
      expect(bitmap.hasRange(50, 54)).toBe(true)
      expect(bitmap.hasRange(50, 55)).toBe(false)

      expect(bitmap.hasRange(50, 52.1)).toBe(true)
      expect(bitmap.hasRange(50, 52.6)).toBe(true)
      expect(bitmap.hasRange(50, 53.1)).toBe(true)
      expect(bitmap.hasRange(50, 53.6)).toBe(true)
      expect(bitmap.hasRange(50, 54.1)).toBe(false)
      expect(bitmap.hasRange(50, 54.6)).toBe(false)
      expect(bitmap.hasRange(50.3, 52.4)).toBe(true)
      expect(bitmap.hasRange(50.7, 52.7)).toBe(true)

      expect(bitmap.hasRange(49.9, 52)).toBe(true)
      expect(bitmap.hasRange(49.9, 52.2)).toBe(true)
      expect(bitmap.hasRange(49.9, 52.6)).toBe(true)
      expect(bitmap.hasRange(49.9, 53.6)).toBe(true)
      expect(bitmap.hasRange(49.9, 54.6)).toBe(false)

      expect(bitmap.hasRange(55, 55.2)).toBe(true)
      expect(bitmap.hasRange(55, 55.5)).toBe(true)
      expect(bitmap.hasRange(55, 55.7)).toBe(true)
    })

    it('works with 0xFFFFFFFF', () => {
      const bitmap = new RoaringBitmap32([4294967295])
      expect(bitmap.hasRange(4294967295, 4294967296)).toBe(true)
      expect(bitmap.hasRange(4294967294, 4294967296)).toBe(false)
      expect(bitmap.hasRange(4294967295, 4294967297)).toBe(false)
      expect(bitmap.hasRange(4294967294, 4294967297)).toBe(false)
    })

    it('works with 0xFFFFFFFE and 0xFFFFFFFF', () => {
      const bitmap = new RoaringBitmap32([4294967294, 4294967295])
      expect(bitmap.has(4294967293)).toBe(false)
      expect(bitmap.has(4294967294)).toBe(true)
      expect(bitmap.has(4294967295)).toBe(true)
      expect(bitmap.hasRange(4294967295, 4294967296)).toBe(true)
      expect(bitmap.hasRange(4294967294, 4294967295)).toBe(true)
      expect(bitmap.hasRange(4294967293, 4294967294)).toBe(false)
      expect(bitmap.hasRange(4294967295, 4294967296)).toBe(true)
      expect(bitmap.hasRange(4294967294, 4294967297)).toBe(false)
      expect(bitmap.hasRange(4294967293, 4294967296)).toBe(false)
      expect(bitmap.hasRange(4294967293, 4294967297)).toBe(false)
    })

    it('works with 0xFFFFFFFD, 0xFFFFFFFE and 0xFFFFFFFF', () => {
      const bitmap = new RoaringBitmap32([4294967293, 4294967294, 4294967295])
      expect(bitmap.has(4294967292)).toBe(false)
      expect(bitmap.has(4294967293)).toBe(true)
      expect(bitmap.has(4294967294)).toBe(true)
      expect(bitmap.has(4294967295)).toBe(true)

      expect(bitmap.hasRange(4294967292, 4294967293)).toBe(false)
      expect(bitmap.hasRange(4294967292, 4294967294)).toBe(false)
      expect(bitmap.hasRange(4294967292, 4294967295)).toBe(false)
      expect(bitmap.hasRange(4294967292, 4294967296)).toBe(false)

      expect(bitmap.hasRange(4294967293, 4294967297)).toBe(false)
      expect(bitmap.hasRange(4294967294, 4294967297)).toBe(false)
      expect(bitmap.hasRange(4294967295, 4294967297)).toBe(false)
      expect(bitmap.hasRange(4294967296, 4294967297)).toBe(false)

      expect(bitmap.hasRange(4294967293, 4294967294)).toBe(true)
      expect(bitmap.hasRange(4294967293, 4294967295)).toBe(true)
      expect(bitmap.hasRange(4294967293, 4294967296)).toBe(true)

      expect(bitmap.hasRange(4294967294, 4294967295)).toBe(true)
      expect(bitmap.hasRange(4294967294, 4294967296)).toBe(true)

      expect(bitmap.hasRange(4294967295, 4294967296)).toBe(true)
    })
  })

  describe('rangeCardinality', () => {
    it('returns the valid size for the basic test', () => {
      const s = 65536
      const r = new RoaringBitmap32()
      r.addRange(s * 2, s * 10)

      // single container (minhb == maxhb)
      expect(r.rangeCardinality(s * 2, s * 3)).toBe(s)
      expect(r.rangeCardinality(s * 2 + 100, s * 3)).toBe(s - 100)
      expect(r.rangeCardinality(s * 2, s * 3 - 200)).toBe(s - 200)
      expect(r.rangeCardinality(s * 2 + 100, s * 3 - 200)).toBe(s - 300)
      // multiple containers (maxhb > minhb)
      expect(r.rangeCardinality(s * 2, s * 5)).toBe(s * 3)
      expect(r.rangeCardinality(s * 2 + 100, s * 5)).toBe(s * 3 - 100)
      expect(r.rangeCardinality(s * 2, s * 5 - 200)).toBe(s * 3 - 200)
      expect(r.rangeCardinality(s * 2 + 100, s * 5 - 200)).toBe(s * 3 - 300)
      // boundary checks
      expect(r.rangeCardinality(s * 20, s * 21)).toBe(0)
      expect(r.rangeCardinality(100, 100)).toBe(0)
      expect(r.rangeCardinality(0, s * 7)).toBe(s * 5)
      expect(r.rangeCardinality(s * 7, Number.MAX_SAFE_INTEGER)).toBe(s * 3)
      // Extremes
      expect(r.rangeCardinality(s * 7, Number.POSITIVE_INFINITY)).toBe(s * 3)
      expect(r.rangeCardinality(-1, Number.POSITIVE_INFINITY)).toBe(r.size)
      expect(r.rangeCardinality(Number.NEGATIVE_INFINITY, Number.POSITIVE_INFINITY)).toBe(r.size)
    })
  })

  describe('addRange', () => {
    it('does nothing for invalid values', () => {
      const bitmap = new RoaringBitmap32([1, 2])
      bitmap.addRange(undefined as any, 6)
      bitmap.addRange(null as any, 6)
      bitmap.addRange('0' as any, 6)
      bitmap.addRange(0, [8] as any)
      bitmap.addRange(NaN, 4)
      bitmap.addRange(4, 4)
      bitmap.addRange(-2, -4)
      bitmap.addRange(-4, -2)
      bitmap.addRange(4, 2)
      bitmap.addRange(4, -2)
      expect(bitmap.toArray()).toEqual([1, 2])
    })

    it('flips a ranges on an empty bitmap', () => {
      const bitmap = new RoaringBitmap32()
      bitmap.addRange(1, 3)
      bitmap.addRange(3, 5)
      bitmap.addRange(8, 10)
      expect(bitmap.toArray()).toEqual([1, 2, 3, 4, 8, 9])
    })

    it('accepts negative numbers', () => {
      const bitmap = new RoaringBitmap32([1])
      bitmap.addRange(-4, 3)
      expect(bitmap.toArray()).toEqual([0, 1, 2])
    })

    it('accepts negative infinity', () => {
      const bitmap = new RoaringBitmap32([1])
      bitmap.addRange(-Infinity, 3)
      expect(bitmap.toArray()).toEqual([0, 1, 2])
    })

    it('adds ranges', () => {
      const bitmap = new RoaringBitmap32([4, 6, 7])
      bitmap.addRange(1, 3)
      bitmap.addRange(3, 5)
      expect(bitmap.toArray()).toEqual([1, 2, 3, 4, 6, 7])
      bitmap.addRange(8, 10)
      bitmap.addRange(4, 10)
      expect(bitmap.toArray()).toEqual([1, 2, 3, 4, 5, 6, 7, 8, 9])
      bitmap.addRange(4, 10)
      expect(bitmap.toArray()).toEqual([1, 2, 3, 4, 5, 6, 7, 8, 9])
    })

    it('adds 0xFFFFFFFF', () => {
      const bitmap = new RoaringBitmap32()
      bitmap.addRange(4294967295, 4294967296)
      expect(bitmap.has(4294967294)).toBe(false)
      expect(bitmap.has(4294967295)).toBe(true)
    })

    it('adds 0xFFFFFFFE and 0xFFFFFFFF', () => {
      const bitmap = new RoaringBitmap32()
      bitmap.addRange(4294967294, 4294967296)
      expect(bitmap.has(4294967293)).toBe(false)
      expect(bitmap.has(4294967294)).toBe(true)
      expect(bitmap.has(4294967295)).toBe(true)
    })

    it('supports very big numbers', () => {
      const bitmap = new RoaringBitmap32()
      bitmap.addRange(4294967294, 9000000000)
      expect(bitmap.has(4294967294)).toBe(true)
      expect(bitmap.has(4294967295)).toBe(true)
    })

    it('supports positive infinity', () => {
      const bitmap = new RoaringBitmap32()
      bitmap.addRange(4294967294, Infinity)
      expect(bitmap.has(4294967294)).toBe(true)
      expect(bitmap.has(4294967295)).toBe(true)
    })

    it('enumerates correctly when adding a range the end', () => {
      const bitmap = new RoaringBitmap32()
      const iterator = new RoaringBitmap32Iterator(bitmap)
      bitmap.addRange(4294967295, 4294967296)
      expect(iterator.next()).toEqual({ done: false, value: 4294967295 })
      expect(iterator.next()).toEqual({ done: true, value: undefined })
    })
  })

  describe('removeRange', () => {
    it('does nothing for invalid values', () => {
      const values = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
      const bitmap = new RoaringBitmap32(values)
      bitmap.removeRange(null as any, 10)
      bitmap.removeRange(undefined as any, 10)
      bitmap.removeRange(-19, -10)
      bitmap.removeRange(-10, -19)
      bitmap.removeRange(10, 3)
      bitmap.removeRange(0, 0)
      bitmap.removeRange(5, 5)
      bitmap.removeRange(5, undefined as any)
      bitmap.removeRange(5, 'xxx' as any)
      expect(bitmap.toArray()).toEqual(values)
    })

    it('removes a range', () => {
      const bitmap = new RoaringBitmap32([1, 2, 3, 4, 5, 6, 7, 8, 9, 10])
      bitmap.removeRange(3, 7)
      expect(bitmap.toArray()).toEqual([1, 2, 7, 8, 9, 10])
    })
  })

  describe('flipRange', () => {
    it('does nothing for invalid values', () => {
      const bitmap = new RoaringBitmap32([1, 2])
      bitmap.flipRange(undefined as any, 6)
      bitmap.flipRange(null as any, 6)
      bitmap.flipRange('0' as any, 6)
      bitmap.flipRange(0, [8] as any)
      bitmap.flipRange(NaN, 4)
      bitmap.flipRange(4, 4)
      bitmap.flipRange(-2, -4)
      bitmap.flipRange(-4, -2)
      bitmap.flipRange(4, 2)
      bitmap.flipRange(4, -2)
      expect(bitmap.toArray()).toEqual([1, 2])
    })

    it('flips a ranges on an empty bitmap', () => {
      const bitmap = new RoaringBitmap32()
      bitmap.flipRange(1, 3)
      bitmap.flipRange(3, 5)
      bitmap.flipRange(8, 10)
      expect(bitmap.toArray()).toEqual([1, 2, 3, 4, 8, 9])
    })

    it('accepts negative numbers', () => {
      const bitmap = new RoaringBitmap32([1])
      bitmap.flipRange(-4, 3)
      expect(bitmap.toArray()).toEqual([0, 2])
    })

    it('accepts negative infinity', () => {
      const bitmap = new RoaringBitmap32([1])
      bitmap.flipRange(-Infinity, 3)
      expect(bitmap.toArray()).toEqual([0, 2])
    })

    it('flips a ranges', () => {
      const bitmap = new RoaringBitmap32([4, 6, 7])
      bitmap.flipRange(1, 3)
      bitmap.flipRange(3, 5)
      bitmap.flipRange(8, 10)
      bitmap.flipRange(4, 10)
      expect(bitmap.toArray()).toEqual([1, 2, 3, 4, 5])
      bitmap.flipRange(4, 10)
      expect(bitmap.toArray()).toEqual([1, 2, 3, 6, 7, 8, 9])
    })

    it('flips 0xFFFFFFFF', () => {
      const bitmap = new RoaringBitmap32()
      bitmap.flipRange(4294967295, 4294967296)
      expect(bitmap.has(4294967294)).toBe(false)
      expect(bitmap.has(4294967295)).toBe(true)
    })

    it('flips 0xFFFFFFFE and 0xFFFFFFFF', () => {
      const bitmap = new RoaringBitmap32()
      bitmap.flipRange(4294967294, 4294967296)
      expect(bitmap.has(4294967293)).toBe(false)
      expect(bitmap.has(4294967294)).toBe(true)
      expect(bitmap.has(4294967295)).toBe(true)
    })

    it('supports very big numbers', () => {
      const bitmap = new RoaringBitmap32()
      bitmap.flipRange(4294967294, 9000000000)
      expect(bitmap.has(4294967294)).toBe(true)
      expect(bitmap.has(4294967295)).toBe(true)
    })

    it('supports positive infinity', () => {
      const bitmap = new RoaringBitmap32()
      bitmap.flipRange(4294967294, Infinity)
      expect(bitmap.has(4294967294)).toBe(true)
      expect(bitmap.has(4294967295)).toBe(true)
    })

    it('enumerates correctly when flipping a range at the end', () => {
      const bitmap = new RoaringBitmap32()
      const iterator = new RoaringBitmap32Iterator(bitmap)
      bitmap.flipRange(4294967295, 4294967296)
      expect(iterator.next()).toEqual({ done: false, value: 4294967295 })
      expect(iterator.next()).toEqual({ done: true, value: undefined })
    })
  })
})
