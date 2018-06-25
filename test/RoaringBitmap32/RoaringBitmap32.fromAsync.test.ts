import RoaringBitmap32 = require('../../RoaringBitmap32')

describe('RoaringBitmap32 fromAsync', () => {
  it('Throws when called with a RoaringBitmap32', async () => {
    await expect(() => RoaringBitmap32.fromArrayAsync(new RoaringBitmap32())).toThrowError()
  })

  it('Throws when called with a RoaringBitmap32', async () => {
    await expect(() => RoaringBitmap32.fromArrayAsync(new RoaringBitmap32(), () => {})).toThrowError()
  })

  describe('awayt/async', () => {
    describe('empty', () => {
      it('creates an empty bitmap with undefined', async () => {
        const bitmap = await RoaringBitmap32.fromArrayAsync(undefined as any)
        expect(bitmap).toBeInstanceOf(RoaringBitmap32)
        expect(bitmap.isEmpty).toBe(true)
      })

      it('creates an empty bitmap with null', async () => {
        const bitmap = await RoaringBitmap32.fromArrayAsync(null as any)
        expect(bitmap).toBeInstanceOf(RoaringBitmap32)
        expect(bitmap.isEmpty).toBe(true)
      })

      it('creates an empty bitmap with an empty Uint32Array', async () => {
        const bitmap = await RoaringBitmap32.fromArrayAsync(new Uint32Array(0))
        expect(bitmap).toBeInstanceOf(RoaringBitmap32)
        expect(bitmap.isEmpty).toBe(true)
      })

      it('creates an empty bitmap with an empty Int32Array', async () => {
        const bitmap = await RoaringBitmap32.fromArrayAsync(new Int32Array(0))
        expect(bitmap).toBeInstanceOf(RoaringBitmap32)
        expect(bitmap.isEmpty).toBe(true)
      })

      it('creates an empty bitmap with an empty array', async () => {
        const bitmap = await RoaringBitmap32.fromArrayAsync([])
        expect(bitmap).toBeInstanceOf(RoaringBitmap32)
        expect(bitmap.isEmpty).toBe(true)
      })
    })

    describe('with data', () => {
      it('creates a bitmap with an Uint32Array', async () => {
        const values = [1, 2, 3, 4, 5]
        const bitmap = await RoaringBitmap32.fromArrayAsync(new Uint32Array(values))
        expect(bitmap).toBeInstanceOf(RoaringBitmap32)
        expect(bitmap.toArray()).toEqual(values)
      })

      it('creates a bitmap with an Int32Array', async () => {
        const values = [1, 2, 3, 4, 5]
        const bitmap = await RoaringBitmap32.fromArrayAsync(new Int32Array(values))
        expect(bitmap).toBeInstanceOf(RoaringBitmap32)
        expect(bitmap.toArray()).toEqual(values)
      })

      it('creates a bitmap with an array', async () => {
        const values = [1, 2, 3, 0x7fffffff, 0xffffffff]
        const bitmap = await RoaringBitmap32.fromArrayAsync(values)
        expect(bitmap).toBeInstanceOf(RoaringBitmap32)
        expect(bitmap.toArray()).toEqual(values)
      })
    })
  })
})
