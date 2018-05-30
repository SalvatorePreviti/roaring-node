import RoaringBitmap32 = require('../../RoaringBitmap32')
import RoaringBitmap32Iterator = require('../../RoaringBitmap32Iterator')

describe('RoaringBitmap32 empty', () => {
  describe('constructor', () => {
    it('is a class', () => {
      expect(typeof RoaringBitmap32).toBe('function')
    })

    it('creates an empty bitmap with no arguments', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.size).toBe(0)
      expect(bitmap.isEmpty).toBe(true)
    })

    it('creates an empty bitmap with an empty array', () => {
      const bitmap = new RoaringBitmap32([])
      expect(bitmap.size).toBe(0)
      expect(bitmap.isEmpty).toBe(true)
    })

    it('can be called as a normal function', () => {
      const bitmap = ((RoaringBitmap32 as any) as () => RoaringBitmap32)()
      expect(bitmap).toBeInstanceOf(RoaringBitmap32)
    })
  })

  describe('has', () => {
    it('is a function', () => {
      const bitmap = new RoaringBitmap32()
      expect(typeof bitmap.has).toBe('function')
    })

    it('returns false with some random ids', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.has(0)).toBe(false)
      expect(bitmap.has(12340)).toBe(false)
      expect(bitmap.has(0xffffffff)).toBe(false)
      expect(bitmap.has(0xffffffff >>> 0)).toBe(false)
    })

    it('returns false with invalid values', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.has(0.5)).toBe(false)
      expect(bitmap.has(-12340)).toBe(false)
      expect(bitmap.has('xxxx' as any)).toBe(false)
      expect(bitmap.has(null as any)).toBe(false)
      expect(bitmap.has(undefined as any)).toBe(false)
      expect(bitmap.has(bitmap as any)).toBe(false)
    })
  })

  describe('minimum', () => {
    it('returns 4294967295', () => {
      const bitmap = new RoaringBitmap32()
      expect(typeof bitmap.minimum).toBe('function')
      expect(bitmap.minimum()).toBe(4294967295)
    })
  })

  describe('maximum', () => {
    it('returns 0', () => {
      const bitmap = new RoaringBitmap32()
      expect(typeof bitmap.maximum).toBe('function')
      expect(bitmap.maximum()).toBe(0)
    })
  })

  describe('get iterator', () => {
    it('has iterator function', () => {
      const bitmap = new RoaringBitmap32()
      expect(typeof bitmap[Symbol.iterator]).toBe('function')
    })

    it('returns a RoaringBitmap32Iterator', () => {
      const bitmap = new RoaringBitmap32()
      const iterator = bitmap[Symbol.iterator]()
      expect(iterator).toBeInstanceOf(RoaringBitmap32Iterator)
      expect(typeof iterator.next).toBe('function')
    })

    it('returns an empty iterator', () => {
      const bitmap = new RoaringBitmap32()
      const iterator = bitmap[Symbol.iterator]()
      expect(iterator.next()).toEqual({
        done: true,
        value: undefined
      })
      expect(iterator.next()).toEqual({
        done: true,
        value: undefined
      })
    })
  })
})
