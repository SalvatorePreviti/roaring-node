import roaring from '..'

import RoaringBitmap32 from '../RoaringBitmap32'
import RoaringBitmap32Iterator from '../RoaringBitmap32Iterator'

describe('roaring', () => {
  it('is an object', () => {
    expect(typeof roaring).toBe('object')
  })

  it('exports itself with a "default" property', () => {
    const required = require('../')
    expect(required).toBeTruthy()
    expect(required === roaring).toBeTruthy()
    expect(required === required.default).toBeTruthy()
  })

  it('has RoaringBitmap32', () => {
    expect(typeof roaring.RoaringBitmap32).toBe('function')
    expect(roaring.RoaringBitmap32 === RoaringBitmap32).toBeTruthy()
  })

  it('has RoaringBitmap32Iterator', () => {
    expect(typeof roaring.RoaringBitmap32Iterator).toBe('function')
    expect(roaring.RoaringBitmap32Iterator === RoaringBitmap32Iterator).toBeTruthy()
  })

  it('has CRoaringVersion', () => {
    expect(typeof roaring.CRoaringVersion).toBe('string')
    const values = roaring.CRoaringVersion.split('.')
    expect(values).toHaveLength(3)
    for (let i = 0; i < 3; ++i) {
      expect(Number.isInteger(Number.parseInt(values[i], 10))).toBe(true)
    }
  })

  it('has roaring PackageVersion', () => {
    expect(typeof roaring.PackageVersion).toBe('string')
    const values = roaring.CRoaringVersion.split('.')
    expect(values).toHaveLength(3)
    for (let i = 0; i < 3; ++i) {
      expect(Number.isInteger(Number.parseInt(values[i], 10))).toBe(true)
    }
  })
})
