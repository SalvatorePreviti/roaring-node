import RoaringBitmap32 from '../../RoaringBitmap32'
import { expect } from 'chai'

describe('RoaringBitmap32 import', () => {
  it('exports itself with a "default" property', () => {
    const required = require('../../RoaringBitmap32')
    expect(!!required).to.be.true
    expect(required === required.default).to.be.true
  })

  it('supports typescript "import RoaringBitmap32 from \'roaring/RoaringBitmap32\'" syntax', () => {
    expect(RoaringBitmap32 === require('../../RoaringBitmap32')).eq(true)
  })

  it('is a class', () => {
    expect(typeof RoaringBitmap32).eq('function')
    expect(RoaringBitmap32.prototype.constructor).eq(RoaringBitmap32)
  })

  it('can be called as a normal function', () => {
    const bitmap = (RoaringBitmap32 as any as () => any)()
    expect(bitmap).to.be.instanceOf(RoaringBitmap32)
  })
})
