import RoaringBitmap32Iterator from '../../RoaringBitmap32Iterator'

describe('RoaringBitmap32Iterator import', () => {
  it('exports itself with a "default" property', () => {
    const required = require('../../RoaringBitmap32Iterator')
    expect(required).toBeTruthy()
    expect(required === required.default).toBeTruthy()
  })

  it('supports typescript "import RoaringBitmap32Iterator from \'roaring/RoaringBitmap32Iterator\'" syntax', () => {
    expect(RoaringBitmap32Iterator === require('../../RoaringBitmap32Iterator'))
  })

  it('is a class', () => {
    expect(typeof RoaringBitmap32Iterator).toBe('function')
    expect(RoaringBitmap32Iterator.prototype.constructor).toBe(RoaringBitmap32Iterator)
  })
})
