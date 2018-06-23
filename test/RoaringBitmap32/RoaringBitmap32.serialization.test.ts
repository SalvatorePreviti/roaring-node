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

  describe('deserializeAsync', () => {
    it('deserializes an empty buffer', async () => {
      const promises: Promise<RoaringBitmap32>[] = []

      for (let i = 0; i < 15; ++i) {
        promises.push(RoaringBitmap32.deserializeAsync(Buffer.from([]), false))
        promises.push(RoaringBitmap32.deserializeAsync(Buffer.from([]), true))
      }

      const resolved = await Promise.all(promises)

      for (const bitmap of resolved) {
        expect(bitmap).toBeInstanceOf(RoaringBitmap32)
        expect(bitmap.size).toBe(0)
      }
    })

    it('deserializes a small bitmap', async () => {
      const values = [1, 2, 100, 101, 105, 109, 0x7fffffff, 0xfffffffe, 0xffffffff]

      const bufferNonPortable = new RoaringBitmap32(values).serialize(false)
      const bufferPortable = new RoaringBitmap32(values).serialize(true)

      const promises: Promise<RoaringBitmap32>[] = []
      for (let i = 0; i < 15; ++i) {
        promises.push(RoaringBitmap32.deserializeAsync(bufferNonPortable, false))
        promises.push(RoaringBitmap32.deserializeAsync(bufferPortable, true))
      }

      const resolved = await Promise.all(promises)

      for (const bitmap of resolved) {
        expect(bitmap).toBeInstanceOf(RoaringBitmap32)
        expect(bitmap.toArray()).toEqual(values)
      }
    })

    it('deserializes a large bitmap', async () => {
      const bitmap = new RoaringBitmap32()
      let rnd = 112043213
      for (let i = 0; i < 100; ++i) {
        rnd = ((rnd + i * 25253) * 3924461) >>> 0
        bitmap.add(rnd)
      }
      const bufferNonPortable = bitmap.serialize(false)
      const bufferPortable = bitmap.serialize(true)

      const promises: Promise<RoaringBitmap32>[] = []
      //for (let i = 0; i < 15; ++i) {
      //promises.push(RoaringBitmap32.deserializeAsync(bufferNonPortable, false))
      promises.push(RoaringBitmap32.deserializeAsync(bufferPortable, true))
      //}

      const resolved = await Promise.all(promises)

      for (const b of resolved) {
        expect(b).toBeInstanceOf(RoaringBitmap32)
        expect(b.isEqual(bitmap)).toBe(true)
      }
    })
  })

  /*

  describe('deserializeAsync', () => {
    describe('async/await', () => {
      describe('empty buffer', () => {
        it('deserializes an empty buffer (non portable, implicit)', async () => {
          const bitmap = await RoaringBitmap32.deserializeAsync(Buffer.from([]))
          expect(bitmap).toBeInstanceOf(RoaringBitmap32)
          expect(bitmap.size).toBe(0)
        })

        it('deserializes an empty buffer (non portable, explicit)', async () => {
          const bitmap = await RoaringBitmap32.deserializeAsync(Buffer.from([]), false)
          expect(bitmap).toBeInstanceOf(RoaringBitmap32)
          expect(bitmap.size).toBe(0)
        })

        it('deserializes an empty buffer (portable)', async () => {
          const bitmap = await RoaringBitmap32.deserializeAsync(Buffer.from([]), true)
          expect(bitmap).toBeInstanceOf(RoaringBitmap32)
          expect(bitmap.size).toBe(0)
        })
      })

      describe('empty bitmap', () => {
        it('deserializes an empty bitmap (non portable, implicit)', async () => {
          const bitmap = await RoaringBitmap32.deserializeAsync(Buffer.from([1, 0, 0, 0, 0]))
          expect(bitmap).toBeInstanceOf(RoaringBitmap32)
          expect(bitmap.size).toBe(0)
        })

        it('deserializes an empty bitmap (non portable, explicit)', async () => {
          const bitmap = await RoaringBitmap32.deserializeAsync(Buffer.from([1, 0, 0, 0, 0]), false)
          expect(bitmap).toBeInstanceOf(RoaringBitmap32)
          expect(bitmap.size).toBe(0)
        })

        it('deserializes an empty bitmap (portable)', async () => {
          const bitmap = await RoaringBitmap32.deserializeAsync(Buffer.from([58, 48, 0, 0, 0, 0, 0, 0]), true)
          expect(bitmap).toBeInstanceOf(RoaringBitmap32)
          expect(bitmap.size).toBe(0)
        })
      })

      describe('simple bitmap', () => {
        const values = [1, 2, 100, 101, 105, 109, 0x7fffffff, 0xfffffffe, 0xffffffff]

        it('deserializes (non portable)', async () => {
          const buffer = new RoaringBitmap32(values).serialize(false)
          const bitmapP = RoaringBitmap32.deserializeAsync(buffer, false)
          console.log('in progress...')
          const bitmap = await bitmapP
          console.log('completed')
          expect(bitmap.toArray()).toEqual(values)
        })

        it('deserializes (portable)', async () => {
          const buffer = new RoaringBitmap32(values).serialize(true)
          const bitmap = await RoaringBitmap32.deserializeAsync(buffer, true)
          expect(bitmap.toArray()).toEqual(values)
        })
      })
    })
  })*/

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
