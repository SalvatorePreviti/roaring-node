import RoaringBitmap32 = require('../../RoaringBitmap32')

describe('RoaringBitmap32 deserializeAsync', () => {
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

    it('deserializes simple bitmap', async () => {
      const values = [1, 2, 100, 101, 105, 109, 0x7fffffff, 0xfffffffe, 0xffffffff]

      const bufferNonPortable = new RoaringBitmap32(values).serialize(false)
      const bufferPortable = new RoaringBitmap32(values).serialize(true)

      const promises: Promise<RoaringBitmap32>[] = []
      for (let i = 0; i < 5; ++i) {
        promises.push(RoaringBitmap32.deserializeAsync(bufferNonPortable, false))
        promises.push(RoaringBitmap32.deserializeAsync(bufferPortable, true))
      }

      const resolved = await Promise.all(promises)

      for (const bitmap of resolved) {
        expect(bitmap).toBeInstanceOf(RoaringBitmap32)
        expect(bitmap.toArray()).toEqual(values)
      }
    })

    it('deserializes larger bitmap', async () => {
      const bitmap = new RoaringBitmap32()
      let rnd = 112043213
      for (let i = 0; i < 1000; ++i) {
        rnd = ((rnd + i * 25253) * 3924461) >>> 0
        bitmap.add(rnd)
      }
      const bufferNonPortable = bitmap.serialize(false)
      const bufferPortable = bitmap.serialize(true)

      const promises: Promise<RoaringBitmap32>[] = []
      for (let i = 0; i < 5; ++i) {
        promises.push(RoaringBitmap32.deserializeAsync(bufferNonPortable, false))
        promises.push(RoaringBitmap32.deserializeAsync(bufferPortable, true))
      }

      const resolved = await Promise.all(promises)

      for (const b of resolved) {
        expect(b).toBeInstanceOf(RoaringBitmap32)
        expect(b.isEqual(bitmap)).toBe(true)
      }
    })

    it('propagates deserialization errors', async () => {
      const wrongBuffer = Buffer.from([99, 98, 97])
      const promise = RoaringBitmap32.deserializeAsync(wrongBuffer, false)
      try {
        await promise
      } catch (e) {
        expect(e.message).toEqual('RoaringBitmap32::deserialize - invalid portable header byte')
      }
    })
  })
})
