import roaring from '../'
import RoaringBitmap32 = require('../RoaringBitmap32')
import RoaringBitmap32Iterator = require('../RoaringBitmap32Iterator')

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
    expect(values.length).toBe(3)
    for (let i = 0; i < 3; ++i) {
      expect(Number.isInteger(Number.parseInt(values[i], 10))).toBe(true)
    }
  })

  it('has roaring PackageVersion', () => {
    expect(typeof roaring.PackageVersion).toBe('string')
    const values = roaring.CRoaringVersion.split('.')
    expect(values.length).toBe(3)
    for (let i = 0; i < 3; ++i) {
      expect(Number.isInteger(Number.parseInt(values[i], 10))).toBe(true)
    }
  })

  it('has AVX2 boolean property', () => {
    expect(typeof roaring.AVX2).toBe('boolean')
  })

  it('has SSE42 boolean property', () => {
    expect(typeof roaring.SSE42).toBe('boolean')
  })

  describe('instructionSet has a valid value', () => {
    switch (roaring.instructionSet) {
      case 'AVX2':
      case 'SSE42':
      case 'PLAIN':
        break
      default:
        throw new Error(`roaring.instructionSet must be AVX2, SSE42 or PLAIN but is ${roaring.instructionSet}`)
    }
  })

  if (process.env.ROARING_TEST_EXPECTED_CPU) {
    it('AVX2/SSE42 matches the expected value', () => {
      expect(roaring.instructionSet).toBe(process.env.ROARING_TEST_EXPECTED_CPU)

      let architecture
      if (roaring.AVX2) {
        architecture = 'AVX2'
        expect(roaring.SSE42).toBe(true)
      } else if (roaring.SSE42) {
        architecture = 'SSE42'
      } else {
        architecture = 'PLAIN'
      }

      expect(architecture).toBe(process.env.ROARING_TEST_EXPECTED_CPU)
    })
  }
})
