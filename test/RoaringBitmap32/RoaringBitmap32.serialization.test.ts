import RoaringBitmap32 = require('../../RoaringBitmap32')

describe('RoaringBitmap32 serialization', () => {
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

    it('returns the correct amount of bytes (non portable)', () => {
      const bitmap = new RoaringBitmap32([1, 2, 3, 4, 5, 6, 100, 101, 105, 109, 0x7fffffff, 0xfffffffe, 0xffffffff])
      expect(bitmap.getSerializationSizeInBytes(false)).toEqual(bitmap.serialize(false).byteLength)
      bitmap.runOptimize()
      bitmap.shrinkToFit()
      expect(bitmap.getSerializationSizeInBytes(false)).toEqual(bitmap.serialize(false).byteLength)
    })

    it('returns the correct amount of bytes (portable)', () => {
      const bitmap = new RoaringBitmap32([1, 2, 3, 4, 5, 6, 100, 101, 105, 109, 0x7fffffff, 0xfffffffe, 0xffffffff])
      expect(bitmap.getSerializationSizeInBytes(true)).toEqual(bitmap.serialize(true).byteLength)
      bitmap.runOptimize()
      bitmap.shrinkToFit()
      expect(bitmap.getSerializationSizeInBytes(true)).toEqual(bitmap.serialize(true).byteLength)
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
    it('deserializes zero length buffer (non portable)', () => {
      const bitmap = new RoaringBitmap32([1, 2, 3])
      bitmap.deserialize(Buffer.from([]))
      expect(bitmap.size).toBe(0)
      expect(bitmap.isEmpty).toBe(true)
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

    it('deserializes zero length buffer (portable)', () => {
      const bitmap = new RoaringBitmap32([1, 2])
      bitmap.deserialize(Buffer.from([]), true)
      expect(bitmap.size).toBe(0)
      expect(bitmap.isEmpty).toBe(true)
    })

    it('is able to deserialize test data', () => {
      const testDataSerialized = require('./data/serialized.json')

      let total = 0
      for (const s of testDataSerialized) {
        const bitmap = RoaringBitmap32.deserialize(Buffer.from(s, 'base64'))
        const size = bitmap.size
        if (size !== 0) {
          expect(bitmap.has(bitmap.minimum())).toBe(true)
          expect(bitmap.has(bitmap.maximum())).toBe(true)
        }
        total += size
      }
      expect(total).toBe(68031)
    })
  })

  describe('deserialize static', () => {
    it('deserializes zero length buffer (non portable)', () => {
      const bitmap = RoaringBitmap32.deserialize(Buffer.from([]))
      expect(bitmap.size).toBe(0)
      expect(bitmap.isEmpty).toBe(true)
    })

    it('deserializes zero length buffer (portable)', () => {
      const bitmap = RoaringBitmap32.deserialize(Buffer.from([]), true)
      expect(bitmap.size).toBe(0)
      expect(bitmap.isEmpty).toBe(true)
    })

    it('deserializes empty bitmap (non portable)', () => {
      const bitmap = RoaringBitmap32.deserialize(Buffer.from([1, 0, 0, 0, 0]))
      expect(bitmap.size).toBe(0)
      expect(bitmap.isEmpty).toBe(true)
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

  describe('serialize, deserialize', () => {
    it('is able to serialize and deserialize data (non portable)', () => {
      const values = [1, 2, 100, 101, 105, 109, 0x7fffffff, 0xfffffffe, 0xffffffff]
      const a = new RoaringBitmap32(values)
      const b = RoaringBitmap32.deserialize(a.serialize(false), false)
      expect(b.toArray()).toEqual(values)
    })

    it('is able to serialize and deserialize data (portable)', () => {
      const values = [1, 2, 100, 101, 105, 109, 0x7fffffff, 0xfffffffe, 0xffffffff]
      const a = new RoaringBitmap32(values)
      const b = RoaringBitmap32.deserialize(a.serialize(true), true)
      expect(b.toArray()).toEqual(values)
    })
  })
})
