import RoaringBitmap32 from '../../RoaringBitmap32'

describe('RoaringBitmap32 import', () => {
  it('exports itself with a "default" property', () => {
    const required = require('../../RoaringBitmap32')
    expect(required).toBeTruthy()
    expect(required === required.default).toBeTruthy()
  })

  it('supports typescript "import RoaringBitmap32 from \'roaring/RoaringBitmap32\'" syntax', () => {
    expect(RoaringBitmap32 === require('../../RoaringBitmap32')).toBe(true)
  })

  it('is a class', () => {
    expect(typeof RoaringBitmap32).toBe('function')
    expect(RoaringBitmap32.prototype.constructor).toBe(RoaringBitmap32)
  })

  it('can be called as a normal function', () => {
    const bitmap = ((RoaringBitmap32 as any) as () => any)()
    expect(bitmap).toBeInstanceOf(RoaringBitmap32)
  })
})
