import RoaringBitmap32 from '../../RoaringBitmap32'

describe('RoaringBitmap32 serialization', () => {
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
    expect(total).toBe(436164)
  })
})
