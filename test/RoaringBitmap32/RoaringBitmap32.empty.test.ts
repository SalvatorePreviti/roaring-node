import RoaringBitmap32 = require('../../RoaringBitmap32')
import RoaringBitmap32Iterator = require('../../RoaringBitmap32Iterator')

describe('RoaringBitmap32 empty', () => {
  describe('constructor', () => {
    it('is a class', () => {
      expect(typeof RoaringBitmap32).toBe('function')
    })

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

    it('can be called as a normal function', () => {
      const bitmap = ((RoaringBitmap32 as any) as () => RoaringBitmap32)()
      expect(bitmap).toBeInstanceOf(RoaringBitmap32)
    })
  })

  describe('has', () => {
    it('is a function', () => {
      const bitmap = new RoaringBitmap32()
      expect(typeof bitmap.has).toBe('function')
    })

    it('returns false with some random ids', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.has(0)).toBe(false)
      expect(bitmap.has(12340)).toBe(false)
      expect(bitmap.has(0xffffffff)).toBe(false)
      expect(bitmap.has(0xffffffff >>> 0)).toBe(false)
    })

    it('returns false with invalid values', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.has(0.5)).toBe(false)
      expect(bitmap.has(-12340)).toBe(false)
      expect(bitmap.has('xxxx' as any)).toBe(false)
      expect(bitmap.has(null as any)).toBe(false)
      expect(bitmap.has(undefined as any)).toBe(false)
      expect(bitmap.has(bitmap as any)).toBe(false)
    })
  })

  describe('clear, remove', () => {
    it('clear does nothing', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.clear()).toBe(undefined)
      expect(bitmap.size).toBe(0)
    })

    it('remove does nothing', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.remove(19)).toBe(undefined)
      expect(bitmap.size).toBe(0)
    })

    it('delete does nothing', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.delete(19)).toBe(false)
      expect(bitmap.size).toBe(0)
    })

    it('removeMany does nothing', () => {
      const bitmap = new RoaringBitmap32()
      bitmap.removeMany([123])
      expect(bitmap.size).toBe(0)
    })
  })

  describe('minimum', () => {
    it('returns 4294967295', () => {
      const bitmap = new RoaringBitmap32()
      expect(typeof bitmap.minimum).toBe('function')
      expect(bitmap.minimum()).toBe(4294967295)
    })
  })

  describe('maximum', () => {
    it('returns 0', () => {
      const bitmap = new RoaringBitmap32()
      expect(typeof bitmap.maximum).toBe('function')
      expect(bitmap.maximum()).toBe(0)
    })
  })

  describe('get iterator', () => {
    it('has iterator function', () => {
      const bitmap = new RoaringBitmap32()
      expect(typeof bitmap[Symbol.iterator]).toBe('function')
    })

    it('returns a RoaringBitmap32Iterator', () => {
      const bitmap = new RoaringBitmap32()
      const iterator = bitmap[Symbol.iterator]()
      expect(iterator).toBeInstanceOf(RoaringBitmap32Iterator)
      expect(typeof iterator.next).toBe('function')
    })

    it('returns an empty iterator', () => {
      const bitmap = new RoaringBitmap32()
      const iterator = bitmap[Symbol.iterator]()
      expect(iterator.next()).toEqual({
        done: true,
        value: undefined
      })
      expect(iterator.next()).toEqual({
        done: true,
        value: undefined
      })
    })
  })

  describe('isSubset', () => {
    it('has isSubset function', () => {
      const bitmap = new RoaringBitmap32()
      expect(typeof bitmap.isSubset).toBe('function')
    })

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
  })

  describe('isStrictSubset', () => {
    it('has isStrictSubset function', () => {
      const bitmap = new RoaringBitmap32()
      expect(typeof bitmap.isStrictSubset).toBe('function')
    })

    it('returns false for invalid values', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.isStrictSubset(null as any)).toBe(false)
      expect(bitmap.isStrictSubset(undefined as any)).toBe(false)
      expect(bitmap.isStrictSubset(123 as any)).toBe(false)
      expect(bitmap.isStrictSubset([123] as any)).toBe(false)
    })

    it('returns false with itself', () => {
      const bitmap = new RoaringBitmap32()
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
  })

  describe('isEqual', () => {
    it('has isEqual function', () => {
      const bitmap = new RoaringBitmap32()
      expect(typeof bitmap.isEqual).toBe('function')
    })

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
  })

  describe('intersects', () => {
    it('has intersects function', () => {
      const bitmap = new RoaringBitmap32()
      expect(typeof bitmap.intersects).toBe('function')
    })

    it('returns false for invalid values', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.intersects(null as any)).toBe(false)
      expect(bitmap.intersects(undefined as any)).toBe(false)
      expect(bitmap.intersects(123 as any)).toBe(false)
      expect(bitmap.intersects([123] as any)).toBe(false)
    })

    it('returns false with itself', () => {
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
  })

  describe('andCardinality', () => {
    it('has andCardinality function', () => {
      const bitmap = new RoaringBitmap32()
      expect(typeof bitmap.andCardinality).toBe('function')
    })

    it('returns -1 for invalid values', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.andCardinality(null as any)).toBe(-1)
      expect(bitmap.andCardinality(undefined as any)).toBe(-1)
      expect(bitmap.andCardinality(123 as any)).toBe(-1)
      expect(bitmap.andCardinality([123] as any)).toBe(-1)
    })

    it('returns 0 with itself', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.andCardinality(bitmap)).toBe(0)
    })

    it('returns 0 with another empty set', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32()
      expect(a.andCardinality(b)).toBe(0)
    })

    it('returns 0 with another non empty set', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32([1, 2, 3])
      expect(a.andCardinality(b)).toBe(0)
    })
  })

  describe('orCardinality', () => {
    it('has orCardinality function', () => {
      const bitmap = new RoaringBitmap32()
      expect(typeof bitmap.orCardinality).toBe('function')
    })

    it('returns -1 for invalid values', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.orCardinality(null as any)).toBe(-1)
      expect(bitmap.orCardinality(undefined as any)).toBe(-1)
      expect(bitmap.orCardinality(123 as any)).toBe(-1)
      expect(bitmap.orCardinality([123] as any)).toBe(-1)
    })

    it('returns 0 with itself', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.orCardinality(bitmap)).toBe(0)
    })

    it('returns 0 with another empty set', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32()
      expect(a.orCardinality(b)).toBe(0)
    })

    it('returns other set size with another non empty set', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32([1, 2, 3])
      expect(a.orCardinality(b)).toBe(3)
    })
  })

  describe('andNotCardinality', () => {
    it('has andCardinality function', () => {
      const bitmap = new RoaringBitmap32()
      expect(typeof bitmap.andNotCardinality).toBe('function')
    })

    it('returns -1 for invalid values', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.andNotCardinality(null as any)).toBe(-1)
      expect(bitmap.andNotCardinality(undefined as any)).toBe(-1)
      expect(bitmap.andNotCardinality(123 as any)).toBe(-1)
      expect(bitmap.andNotCardinality([123] as any)).toBe(-1)
    })

    it('returns 0 with itself', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.andNotCardinality(bitmap)).toBe(0)
    })

    it('returns 0 with another empty set', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32()
      expect(a.andNotCardinality(b)).toBe(0)
    })

    it('returns 0 with another non empty set', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32([1, 2, 3])
      expect(a.andNotCardinality(b)).toBe(0)
    })
  })

  describe('xorCardinality', () => {
    it('has xorCardinality function', () => {
      const bitmap = new RoaringBitmap32()
      expect(typeof bitmap.andNotCardinality).toBe('function')
    })

    it('returns -1 for invalid values', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.xorCardinality(null as any)).toBe(-1)
      expect(bitmap.xorCardinality(undefined as any)).toBe(-1)
      expect(bitmap.xorCardinality(123 as any)).toBe(-1)
      expect(bitmap.xorCardinality([123] as any)).toBe(-1)
    })

    it('returns 0 with itself', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.xorCardinality(bitmap)).toBe(0)
    })

    it('returns 0 with another empty set', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32()
      expect(a.xorCardinality(b)).toBe(0)
    })

    it('returns other set size with another non empty set', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32([1, 2, 3])
      expect(a.xorCardinality(b)).toBe(3)
    })
  })

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

  describe('coptFrom', () => {
    it('is a function', () => {
      const bitmap = new RoaringBitmap32()
      expect(typeof bitmap.copyFrom).toBe('function')
    })

    it('does nothing when copying self', () => {
      const bitmap = new RoaringBitmap32()
      bitmap.copyFrom(bitmap)
      expect(bitmap.size).toBe(0)
      expect(bitmap.isEmpty).toBe(true)
    })

    it('does nothing when copying an empty array', () => {
      const bitmap = new RoaringBitmap32()
      bitmap.copyFrom([])
      expect(bitmap.size).toBe(0)
      expect(bitmap.isEmpty).toBe(true)
    })

    it('does nothing when copying an empty RoaringBitmap32', () => {
      const bitmap = new RoaringBitmap32()
      bitmap.copyFrom(new RoaringBitmap32())
      expect(bitmap.size).toBe(0)
      expect(bitmap.isEmpty).toBe(true)
    })

    it('clears if passed an empty array', () => {
      const bitmap = new RoaringBitmap32([1, 2, 3])
      bitmap.copyFrom([])
      expect(bitmap.size).toBe(0)
      expect(bitmap.isEmpty).toBe(true)
    })

    it('clears if passed an empty RoaringBitmap32', () => {
      const bitmap = new RoaringBitmap32([1, 2, 3])
      bitmap.copyFrom(new RoaringBitmap32())
      expect(bitmap.size).toBe(0)
      expect(bitmap.isEmpty).toBe(true)
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

  describe('serialize', () => {
    it('returns a Buffer', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.serialize()).toBeInstanceOf(Buffer)
    })

    it('returns standard value for empty bitmap (non portable)', () => {
      const bitmap = new RoaringBitmap32()
      expect(Array.from(bitmap.serialize())).toEqual([1, 0, 0, 0, 0])
      expect(Array.from(bitmap.serialize(false))).toEqual([1, 0, 0, 0, 0])
    })

    it('returns standard value for empty bitmap (portable)', () => {
      const bitmap = new RoaringBitmap32()
      expect(Array.from(bitmap.serialize(true))).toEqual([58, 48, 0, 0, 0, 0, 0, 0])
    })
  })

  describe('deserialize', () => {
    it('is a function', () => {
      const bitmap = new RoaringBitmap32()
      expect(typeof bitmap.deserialize).toBe('function')
    })

    it('deserializes empty bitmap (non portable)', () => {
      const bitmap = new RoaringBitmap32([1, 2, 3])
      bitmap.deserialize(Buffer.from([1, 0, 0, 0, 0]))
      expect(bitmap.size).toBe(0)
      expect(bitmap.isEmpty).toBe(true)
    })

    it('deserializes empty bitmap (non portable, explicit)', () => {
      const bitmap = new RoaringBitmap32([1, 2, 3])
      bitmap.deserialize(Buffer.from([1, 0, 0, 0, 0]), false)
      expect(bitmap.size).toBe(0)
      expect(bitmap.isEmpty).toBe(true)
    })

    it('deserializes empty bitmap (portable)', () => {
      const bitmap = new RoaringBitmap32([1, 2])
      bitmap.deserialize(Buffer.from([58, 48, 0, 0, 0, 0, 0, 0]), true)
      expect(bitmap.size).toBe(0)
      expect(bitmap.isEmpty).toBe(true)
    })
  })

  describe('deserialize static', () => {
    it('is a function', () => {
      expect(typeof RoaringBitmap32.deserialize).toBe('function')
    })

    it('deserializes empty bitmap (non portable)', () => {
      const bitmap = RoaringBitmap32.deserialize(Buffer.from([1, 0, 0, 0, 0]))
      expect(bitmap.size).toBe(0)
      expect(bitmap.isEmpty).toBe(true)
    })

    it('deserializes empty bitmap (non portable, explicit)', () => {
      const bitmap = RoaringBitmap32.deserialize(Buffer.from([1, 0, 0, 0, 0]), false)
      expect(bitmap.size).toBe(0)
      expect(bitmap.isEmpty).toBe(true)
    })

    it('deserializes empty bitmap (portable)', () => {
      const bitmap = RoaringBitmap32.deserialize(Buffer.from([58, 48, 0, 0, 0, 0, 0, 0]), true)
      expect(bitmap.size).toBe(0)
      expect(bitmap.isEmpty).toBe(true)
    })
  })

  describe('rank', () => {
    it('has rank function', () => {
      const bitmap = new RoaringBitmap32()
      expect(typeof bitmap.rank).toBe('function')
    })

    it('returns 0 for invalid values', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.rank(null as any)).toBe(0)
      expect(bitmap.rank(undefined as any)).toBe(0)
      expect(bitmap.rank(-123 as any)).toBe(0)
      expect(bitmap.rank([123] as any)).toBe(0)
    })

    it('returns 0 with any value', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.rank(0)).toBe(0)
      expect(bitmap.rank(100)).toBe(0)
      expect(bitmap.rank(0x7fffffff)).toBe(0)
      expect(bitmap.rank(0xffffffff)).toBe(0)
    })
  })

  describe('toUint32Array', () => {
    it('has toUint32Array function', () => {
      const bitmap = new RoaringBitmap32()
      expect(typeof bitmap.toUint32Array).toBe('function')
    })

    it('returns an empty Uint32Array', () => {
      const bitmap = new RoaringBitmap32()
      const a = bitmap.toUint32Array()
      expect(a).toBeInstanceOf(Uint32Array)
      expect(a.length).toBe(0)
    })
  })
})
