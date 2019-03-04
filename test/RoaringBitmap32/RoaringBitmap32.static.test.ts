import RoaringBitmap32 from '../../RoaringBitmap32'

describe('RoaringBitmap32 static', () => {
  describe('constructor', () => {
    it('creates an empty bitmap with no arguments', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.size).toBe(0)
      expect(bitmap.isEmpty).toBe(true)
    })

    it('creates an empty bitmap with an empty array', () => {
      const bitmap = new RoaringBitmap32([])
      expect(bitmap.size).toBe(0)
      expect(bitmap.isEmpty).toBe(true)
    })

    it('creates an empty bitmap with an empty bitmap', () => {
      const bitmap = new RoaringBitmap32(new RoaringBitmap32())
      expect(bitmap.size).toBe(0)
      expect(bitmap.isEmpty).toBe(true)
    })

    it('construct with an array of values', () => {
      const values = [1, 2, 3, 6, 7, 8, 20, 44444]
      const bitmap = new RoaringBitmap32(values)
      bitmap.runOptimize()
      bitmap.shrinkToFit()
      expect(bitmap.size).toBe(values.length)
      for (const v of values) {
        expect(bitmap.has(v)).toBe(true)
      }
      expect(bitmap.has(5)).toBe(false)
    })

    it('construct with an UInt32Array', () => {
      const values = [1, 2, 3, 6, 7, 8, 20, 44444]
      const bitmap = new RoaringBitmap32(new Uint32Array(values))
      bitmap.runOptimize()
      bitmap.shrinkToFit()
      expect(bitmap.size).toBe(values.length)
      for (const v of values) {
        expect(bitmap.has(v)).toBe(true)
      }
      expect(bitmap.has(5)).toBe(false)
    })

    it('construct with a RoaringBitmap32', () => {
      const values = [1, 2, 3, 6, 7, 8, 20, 44444]
      const bitmap1 = new RoaringBitmap32(values)
      const bitmap2 = new RoaringBitmap32(bitmap1)
      expect(bitmap2.size).toBe(values.length)
      for (const v of values) {
        expect(bitmap2.has(v)).toBe(true)
      }
      expect(bitmap2.has(5)).toBe(false)
    })

    it('works with an array', () => {
      const values = [100, 120, 130, 140, 150, 99, 1, 13, 64]
      const bitmap = new RoaringBitmap32(values)
      for (const value of values) {
        expect(bitmap.has(value)).toBe(true)
      }
      expect(Array.from(bitmap)).toEqual(values.slice().sort((a, b) => a - b))
    })

    it('works with an Uint32Array', () => {
      const values = [100, 120, 130, 140, 150, 99, 1, 13, 64]
      const bitmap = new RoaringBitmap32(new Uint32Array(values))
      for (const value of values) {
        expect(bitmap.has(value)).toBe(true)
      }
      expect(Array.from(bitmap)).toEqual(values.slice().sort((a, b) => a - b))
    })
  })

  describe('static from', () => {
    it('creates an empty bitmap with an empty array', () => {
      const bitmap = RoaringBitmap32.from([])
      expect(bitmap.size).toBe(0)
      expect(bitmap.isEmpty).toBe(true)
    })

    it('creates an empty bitmap with an empty bitmap', () => {
      const bitmap = RoaringBitmap32.from(new RoaringBitmap32())
      expect(bitmap.size).toBe(0)
      expect(bitmap.isEmpty).toBe(true)
    })

    it('creates a bitmap from an array', () => {
      const bitmap = RoaringBitmap32.from([1, 3, 2, 100, 50])
      expect(bitmap.toArray()).toEqual([1, 2, 3, 50, 100])
    })

    it('creates a bitmap from an Uint32Array', () => {
      const bitmap = RoaringBitmap32.from(new Uint32Array([1, 3, 2, 100, 50]))
      expect(bitmap.toArray()).toEqual([1, 2, 3, 50, 100])
    })

    it('creates a bitmap from an Int32Array', () => {
      const bitmap = RoaringBitmap32.from(new Int32Array([1, 3, 2, 100, 50]))
      expect(bitmap.toArray()).toEqual([1, 2, 3, 50, 100])
    })

    it('creates a bitmap from another bitmap', () => {
      const bitmap1 = new RoaringBitmap32([1, 3, 2, 100, 50])
      const bitmap2 = RoaringBitmap32.from(bitmap1)
      expect(bitmap1 !== bitmap2).toBeTruthy()
      expect(bitmap2.toArray()).toEqual([1, 2, 3, 50, 100])
    })
  })

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

    it('creates an empty bitmap with no argument passed', () => {
      const x = RoaringBitmap32.orMany()
      expect(x).toBeInstanceOf(RoaringBitmap32)
      expect(x.size).toBe(0)
      expect(x.isEmpty).toBe(true)
    })

    it('creates an empty bitmap with a single empty bitmap passed', () => {
      const x = RoaringBitmap32.orMany(new RoaringBitmap32())
      expect(x).toBeInstanceOf(RoaringBitmap32)
      expect(x.size).toBe(0)
      expect(x.isEmpty).toBe(true)
    })

    it('creates an empty bitmap with a single empty bitmap passed as array', () => {
      const x = RoaringBitmap32.orMany([new RoaringBitmap32()])
      expect(x).toBeInstanceOf(RoaringBitmap32)
      expect(x.size).toBe(0)
      expect(x.isEmpty).toBe(true)
    })

    it('creates an empty bitmap with multiple empty bitmap passed', () => {
      const x = RoaringBitmap32.orMany(new RoaringBitmap32(), new RoaringBitmap32(), new RoaringBitmap32())
      expect(x).toBeInstanceOf(RoaringBitmap32)
      expect(x.size).toBe(0)
      expect(x.isEmpty).toBe(true)
    })

    it('creates an empty bitmap with a multiple empty bitmap passed as array', () => {
      const x = RoaringBitmap32.orMany([new RoaringBitmap32(), new RoaringBitmap32(), new RoaringBitmap32()])
      expect(x).toBeInstanceOf(RoaringBitmap32)
      expect(x.size).toBe(0)
      expect(x.isEmpty).toBe(true)
    })
  })

  describe('static xorMany', () => {
    it('xorManys multiple bitmaps (spread arguments)', () => {
      const bitmap = RoaringBitmap32.xorMany(
        new RoaringBitmap32([3, 1, 2]),
        new RoaringBitmap32([3, 2]),
        new RoaringBitmap32([6, 7, 8]),
        new RoaringBitmap32([7, 4])
      )
      expect(bitmap.toArray()).toEqual([1, 4, 6, 8])
    })

    it('xorManys multiple bitmaps (array)', () => {
      const bitmap = RoaringBitmap32.xorMany([
        new RoaringBitmap32([3, 1, 2]),
        new RoaringBitmap32([3, 2]),
        new RoaringBitmap32([6, 7, 8]),
        new RoaringBitmap32([7, 4])
      ])
      expect(bitmap.toArray()).toEqual([1, 4, 6, 8])
    })

    it('creates an empty bitmap with no argument passed', () => {
      const x = RoaringBitmap32.xorMany()
      expect(x).toBeInstanceOf(RoaringBitmap32)
      expect(x.size).toBe(0)
      expect(x.isEmpty).toBe(true)
    })

    it('creates an empty bitmap with a single empty bitmap passed', () => {
      const x = RoaringBitmap32.xorMany(new RoaringBitmap32())
      expect(x).toBeInstanceOf(RoaringBitmap32)
      expect(x.size).toBe(0)
      expect(x.isEmpty).toBe(true)
    })

    it('creates an empty bitmap with a single empty bitmap passed as array', () => {
      const x = RoaringBitmap32.xorMany([new RoaringBitmap32()])
      expect(x).toBeInstanceOf(RoaringBitmap32)
      expect(x.size).toBe(0)
      expect(x.isEmpty).toBe(true)
    })

    it('creates an empty bitmap with multiple empty bitmap passed', () => {
      const x = RoaringBitmap32.xorMany(new RoaringBitmap32(), new RoaringBitmap32(), new RoaringBitmap32())
      expect(x).toBeInstanceOf(RoaringBitmap32)
      expect(x.size).toBe(0)
      expect(x.isEmpty).toBe(true)
    })

    it('creates an empty bitmap with a multiple empty bitmap passed as array', () => {
      const x = RoaringBitmap32.xorMany([new RoaringBitmap32(), new RoaringBitmap32(), new RoaringBitmap32()])
      expect(x).toBeInstanceOf(RoaringBitmap32)
      expect(x.size).toBe(0)
      expect(x.isEmpty).toBe(true)
    })
  })

  describe('static swap', () => {
    it('swaps two empty bitmaps', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32()
      RoaringBitmap32.swap(a, b)
      expect(a.size).toBe(0)
      expect(a.isEmpty).toBe(true)
      expect(b.size).toBe(0)
      expect(b.isEmpty).toBe(true)
    })

    it('swaps one empty with a non empty bitmap', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32([1, 2, 3])
      RoaringBitmap32.swap(a, b)
      expect(a.size).toBe(3)
      expect(a.isEmpty).toBe(false)
      expect(b.size).toBe(0)
      expect(b.isEmpty).toBe(true)
    })

    it('swaps two bitmaps', () => {
      const a = new RoaringBitmap32([4, 5])
      const b = new RoaringBitmap32([1, 2, 3])
      RoaringBitmap32.swap(a, b)
      expect(a.size).toBe(3)
      expect(a.isEmpty).toBe(false)
      expect(b.size).toBe(2)
      expect(b.isEmpty).toBe(false)
      expect(a.toArray()).toEqual([1, 2, 3])
      expect(b.toArray()).toEqual([4, 5])
    })
  })
})
