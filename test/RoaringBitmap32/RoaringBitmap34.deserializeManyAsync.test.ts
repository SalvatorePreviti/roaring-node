import RoaringBitmap32 = require('../../RoaringBitmap32')

describe('RoaringBitmap32 deserializeManyAsync', () => {
  describe('async/await', () => {
    describe('one empty buffer', () => {
      it('deserializes an empty buffer (non portable, implicit)', async () => {
        const bitmap = await RoaringBitmap32.deserializeManyAsync([Buffer.from([])])
        expect(bitmap.length).toEqual(1)
        expect(bitmap[0]).toBeInstanceOf(RoaringBitmap32)
        expect(bitmap[0].size).toBe(0)
      })

      it('deserializes an empty buffer (non portable, explicit)', async () => {
        const bitmap = await RoaringBitmap32.deserializeManyAsync([Buffer.from([])], false)
        expect(bitmap.length).toEqual(1)
        expect(bitmap[0]).toBeInstanceOf(RoaringBitmap32)
        expect(bitmap[0].size).toBe(0)
      })

      it('deserializes an empty buffer (portable)', async () => {
        const bitmap = await RoaringBitmap32.deserializeManyAsync([Buffer.from([])], true)
        expect(bitmap.length).toEqual(1)
        expect(bitmap[0]).toBeInstanceOf(RoaringBitmap32)
        expect(bitmap[0].size).toBe(0)
      })
    })

    describe('multiple empty buffers', () => {
      it('deserializes an empty buffer (non portable, implicit)', async () => {
        const bitmap = await RoaringBitmap32.deserializeManyAsync([Buffer.from([]), Buffer.from([]), Buffer.from([])])
        expect(bitmap.length).toEqual(3)
        for (let i = 0; i < 3; ++i) {
          expect(bitmap[i]).toBeInstanceOf(RoaringBitmap32)
          expect(bitmap[i].size).toBe(0)
        }
      })

      it('deserializes an empty buffer (non portable, explicit)', async () => {
        const bitmap = await RoaringBitmap32.deserializeManyAsync([Buffer.from([]), Buffer.from([]), Buffer.from([])], false)
        expect(bitmap.length).toEqual(3)
        for (let i = 0; i < 3; ++i) {
          expect(bitmap[i]).toBeInstanceOf(RoaringBitmap32)
          expect(bitmap[i].size).toBe(0)
        }
      })

      it('deserializes an empty buffer (portable)', async () => {
        const bitmap = await RoaringBitmap32.deserializeManyAsync([Buffer.from([]), Buffer.from([]), Buffer.from([])], true)
        expect(bitmap.length).toEqual(3)
        for (let i = 0; i < 3; ++i) {
          expect(bitmap[i]).toBeInstanceOf(RoaringBitmap32)
          expect(bitmap[i].size).toBe(0)
        }
      })
    })

    it('deserializes multiple buffers (non portable)', async () => {
      const sources = []
      for (let i = 0; i < 10; ++i) {
        sources.push(new RoaringBitmap32([i, i + 10, i * 10000, i * 999999]))
      }

      const result = await RoaringBitmap32.deserializeManyAsync(sources.map(x => x.serialize()))
      expect(result.length).toEqual(sources.length)
      for (let i = 0; i < sources.length; ++i) {
        expect(result[i].toArray()).toEqual(sources[i].toArray())
      }
    })

    it('deserializes multiple buffers (portable)', async () => {
      const sources = []
      for (let i = 0; i < 10; ++i) {
        sources.push(new RoaringBitmap32([i, i + 10, i * 10000, i * 999999]))
      }

      const result = await RoaringBitmap32.deserializeManyAsync(sources.map(x => x.serialize(true)), true)
      expect(result.length).toEqual(sources.length)
      for (let i = 0; i < sources.length; ++i) {
        expect(result[i].toArray()).toEqual(sources[i].toArray())
      }
    })
  })
})
