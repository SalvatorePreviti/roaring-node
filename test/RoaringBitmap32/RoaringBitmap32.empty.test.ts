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

    it('creates an empty bitmap with an empty bitmap', () => {
      const bitmap = new RoaringBitmap32(new RoaringBitmap32())
      expect(bitmap.size).toBe(0)
      expect(bitmap.isEmpty).toBe(true)
    })

    it('can be called as a normal function', () => {
      const bitmap = ((RoaringBitmap32 as any) as () => RoaringBitmap32)()
      expect(bitmap).toBeInstanceOf(RoaringBitmap32)
    })
  })

  describe('from', () => {
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
  })

  describe('has', () => {
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

  describe('andInPlace', () => {
    it('does nothing with an empty bitmaps', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32()
      a.andInPlace(b)
      expect(a.isEmpty).toBe(true)
    })

    it('does nothing with an empty array', () => {
      const a = new RoaringBitmap32()
      a.andInPlace([])
      expect(a.isEmpty).toBe(true)
    })

    it('does nothing with an non empty array', () => {
      const a = new RoaringBitmap32()
      a.andInPlace([1, 2, 3])
      expect(a.isEmpty).toBe(true)
    })
  })

  describe('andNotInPlace', () => {
    it('does nothing with an empty bitmaps', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32()
      a.andNotInPlace(b)
      expect(a.isEmpty).toBe(true)
    })

    it('does nothing with an empty array', () => {
      const a = new RoaringBitmap32()
      a.andNotInPlace([])
      expect(a.isEmpty).toBe(true)
    })

    it('does nothing with an non empty array', () => {
      const a = new RoaringBitmap32()
      a.andNotInPlace([1, 2, 3])
      expect(a.isEmpty).toBe(true)
    })
  })

  describe('orInPlace', () => {
    it('does nothing with an empty bitmaps', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32()
      a.orInPlace(b)
      expect(a.isEmpty).toBe(true)
    })

    it('does nothing with an empty array', () => {
      const a = new RoaringBitmap32()
      a.orInPlace([])
      expect(a.isEmpty).toBe(true)
    })

    it('adds values to an empty bitmap', () => {
      const a = new RoaringBitmap32()
      a.orInPlace([1, 2, 3])
      expect(a.size).toBe(3)
    })
  })

  describe('xorInPlace', () => {
    it('does nothing with an empty bitmaps', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32()
      a.xorInPlace(b)
      expect(a.isEmpty).toBe(true)
    })

    it('does nothing with an empty array', () => {
      const a = new RoaringBitmap32()
      a.xorInPlace([])
      expect(a.isEmpty).toBe(true)
    })

    it('adds values to an empty bitmap', () => {
      const a = new RoaringBitmap32()
      a.xorInPlace([1, 2, 3])
      expect(a.size).toBe(3)
    })

    it('xors', () => {
      const a = new RoaringBitmap32([1, 2, 3])
      a.xorInPlace([1, 2, 3])
      expect(a.size).toBe(0)
    })
  })

  describe('static and', () => {
    it('returns empty with empty bitmaps', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32()
      const c = RoaringBitmap32.and(a, b)
      expect(c.isEmpty).toBe(true)
    })

    it('returns empty with an non empty bitmap', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32([1, 2, 3])
      const c = RoaringBitmap32.and(a, b)
      expect(c.isEmpty).toBe(true)
    })
  })

  describe('static andNot', () => {
    it('returns empty with empty bitmaps', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32()
      const c = RoaringBitmap32.andNot(a, b)
      expect(c.isEmpty).toBe(true)
    })

    it('returns empty  with an non empty array', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32([1, 2, 3])
      const c = RoaringBitmap32.andNot(a, b)
      expect(c.isEmpty).toBe(true)
    })
  })

  describe('static or', () => {
    it('returns empty with empty bitmaps', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32()
      const c = RoaringBitmap32.or(a, b)
      expect(c.isEmpty).toBe(true)
    })

    it('performs or with empty and non empty', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32([1, 2, 3])
      const c = RoaringBitmap32.or(a, b)
      expect(c.size).toBe(3)
    })

    it('performs or with non empty and non empty', () => {
      const a = new RoaringBitmap32([1, 2, 5])
      const b = new RoaringBitmap32([1, 2, 3])
      const c = RoaringBitmap32.or(a, b)
      expect(c.size).toBe(4)
    })
  })

  describe('static xor', () => {
    it('returns empty with empty bitmaps', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32()
      const c = RoaringBitmap32.xor(a, b)
      expect(c.isEmpty).toBe(true)
    })

    it('performs or with empty and non empty', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32([1, 2, 3])
      const c = RoaringBitmap32.xor(a, b)
      expect(c.size).toBe(3)
    })

    it('performs or with non empty and non empty', () => {
      const a = new RoaringBitmap32([1, 2, 5])
      const b = new RoaringBitmap32([1, 2, 3])
      const c = RoaringBitmap32.xor(a, b)
      expect(c.size).toBe(2)
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

  describe('copyFrom', () => {
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

  describe('swap', () => {
    it('is a function', () => {
      expect(typeof RoaringBitmap32.swap).toBe('function')
    })

    it('swaps two empty bitmaps', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32()
      RoaringBitmap32.swap(a, b)
      expect(a.size).toBe(0)
      expect(a.isEmpty).toBe(true)
      expect(b.size).toBe(0)
      expect(b.isEmpty).toBe(true)
    })

    it('swaps two bitmap', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32([1, 2, 3])
      RoaringBitmap32.swap(a, b)
      expect(a.size).toBe(3)
      expect(a.isEmpty).toBe(false)
      expect(b.size).toBe(0)
      expect(b.isEmpty).toBe(true)
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
      expect(new RoaringBitmap32().serialize()).toBeInstanceOf(Buffer)
    })

    it('returns standard value for empty bitmap (non portable)', () => {
      const bitmap = new RoaringBitmap32()
      expect(Array.from(bitmap.serialize())).toEqual([1, 0, 0, 0, 0])
      expect(Array.from(bitmap.serialize(false))).toEqual([1, 0, 0, 0, 0])
    })

    it('returns standard value for empty bitmap (portable)', () => {
      expect(Array.from(new RoaringBitmap32().serialize(true))).toEqual([58, 48, 0, 0, 0, 0, 0, 0])
    })
  })

  describe('deserialize', () => {
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

  describe('select', () => {
    it('returns undefined for invalid values', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.select(null as any)).toBe(undefined)
      expect(bitmap.select(undefined as any)).toBe(undefined)
      expect(bitmap.select(-123 as any)).toBe(undefined)
      expect(bitmap.select([123] as any)).toBe(undefined)
    })

    it('returns undefined with any value', () => {
      const bitmap = new RoaringBitmap32([12, 3, 4])
      expect(bitmap.select(0)).toBe(undefined)
      expect(bitmap.select(100)).toBe(undefined)
      expect(bitmap.select(0x7fffffff)).toBe(undefined)
      expect(bitmap.select(3)).toBe(undefined)
    })
  })

  describe('toArray', () => {
    it('returns an empty array', () => {
      expect(new RoaringBitmap32().toArray()).toEqual([])
    })
  })

  describe('toUint32Array', () => {
    it('returns an empty Uint32Array', () => {
      const a = new RoaringBitmap32().toUint32Array()
      expect(a).toBeInstanceOf(Uint32Array)
      expect(a.length).toBe(0)
    })
  })

  describe('toString', () => {
    it('returns "RoaringBitmap32:0"', () => {
      expect(new RoaringBitmap32().toString()).toBe('RoaringBitmap32:0')
    })
  })

  describe('contentToString', () => {
    it('returns "[]"', () => {
      expect(new RoaringBitmap32().contentToString()).toBe('[]')
    })
  })

  describe('clone', () => {
    it('returns a cloned empty bitmap', () => {
      const bitmap1 = new RoaringBitmap32()
      const bitmap2 = bitmap1.clone()
      expect(bitmap1 !== bitmap2).toBeTruthy()
      expect(bitmap2).toBeInstanceOf(RoaringBitmap32)
      expect(bitmap2.size).toBe(0)
      expect(bitmap2.isEmpty).toBe(true)
    })
  })

  describe('statistics', () => {
    it('returns a statistics object for an empty bitmap', () => {
      const bitmap = new RoaringBitmap32()
      const statistics = bitmap.statistics()
      expect(statistics).toEqual({
        containers: 0,
        arrayContainers: 0,
        runContainers: 0,
        bitsetContainers: 0,
        valuesInArrayContainers: 0,
        valuesInRunContainers: 0,
        valuesInBitsetContainers: 0,
        bytesInArrayContainers: 0,
        bytesInRunContainers: 0,
        bytesInBitsetContainers: 0,
        maxValue: 0,
        minValue: 4294967295,
        sumOfAllValues: 0,
        size: 0
      })
    })
  })

  describe('toJSON', () => {
    it('returns an empty array', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.toJSON()).toEqual([])
    })

    it('works with JSON.stringify', () => {
      const bitmap = new RoaringBitmap32()
      expect(JSON.stringify(bitmap)).toEqual('[]')
    })
  })
})
