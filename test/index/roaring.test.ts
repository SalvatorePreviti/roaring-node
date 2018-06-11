import roaring = require('../../')
import RoaringBitmap32 = require('../../RoaringBitmap32')
import RoaringBitmap32Iterator = require('../../RoaringBitmap32Iterator')

describe('roaring', () => {
  it('is an object', () => {
    expect(typeof roaring).toBe('object')
  })

  it('has RoaringBitmap32', () => {
    expect(typeof roaring.RoaringBitmap32).toBe('function')
    expect(roaring.RoaringBitmap32 === RoaringBitmap32).toBeTruthy()
  })

  it('has RoaringBitmap32', () => {
    expect(typeof roaring.RoaringBitmap32Iterator).toBe('function')
    expect(roaring.RoaringBitmap32Iterator === RoaringBitmap32Iterator).toBeTruthy()
  })

  it('has version', () => {
    expect(typeof roaring.CRoaringVersion).toBe('string')
    const values = roaring.CRoaringVersion.split('.')
    expect(values.length).toBe(3)
    for (let i = 0; i < 3; ++i) {
      expect(Number.isInteger(Number.parseInt(values[i]))).toBe(true)
    }
  })
})
