import RoaringBitmap32 = require('../../RoaringBitmap32')
import RoaringBitmap32Iterator = require('../../RoaringBitmap32Iterator')

describe('RoaringBitmap32Iterator', () => {
  describe('constructor', () => {
    it('is a class', () => {
      expect(typeof RoaringBitmap32).toBe('function')
    })

    it('creates an empty iterator with no arguments', () => {
      const iter = new RoaringBitmap32Iterator()
      expect(iter).toBeInstanceOf(RoaringBitmap32Iterator)
    })

    it('creates an iterator with a RoaringBitmap32', () => {
      const bitmap = new RoaringBitmap32([3, 4, 5])
      const iter = new RoaringBitmap32Iterator(bitmap)
      expect(iter).toBeInstanceOf(RoaringBitmap32Iterator)
    })

    it('throws an exception if called with a non RoaringBitmap32', () => {
      expect(() => new RoaringBitmap32Iterator(123 as any)).toThrowError()
      expect(() => new RoaringBitmap32Iterator([123] as any)).toThrowError()
    })
  })

  describe('next', () => {
    it('is a function', () => {
      const iter = new RoaringBitmap32Iterator()
      expect(typeof iter.next).toBe('function')
    })

    it('returns an empty result if iterator is created without arguments', () => {
      const iter = new RoaringBitmap32Iterator()
      expect(iter.next()).toEqual({ value: undefined, done: true })
      expect(iter.next()).toEqual({ value: undefined, done: true })
    })

    it('returns an empty result if iterator is created with an empty RoaringBitmap32', () => {
      const iter = new RoaringBitmap32Iterator(new RoaringBitmap32())
      expect(iter.next()).toEqual({ value: undefined, done: true })
      expect(iter.next()).toEqual({ value: undefined, done: true })
    })

    it('allows iterating a small array', () => {
      const iter = new RoaringBitmap32Iterator(new RoaringBitmap32([123, 456, 999, 1000]))
      expect(iter.next()).toEqual({ value: 123, done: false })
      expect(iter.next()).toEqual({ value: 456, done: false })
      expect(iter.next()).toEqual({ value: 999, done: false })
      expect(iter.next()).toEqual({ value: 1000, done: false })
      expect(iter.next()).toEqual({ value: undefined, done: true })
      expect(iter.next()).toEqual({ value: undefined, done: true })
      expect(iter.next()).toEqual({ value: undefined, done: true })
    })
  })

  describe('Symbol.iterator', () => {
    it('is a function', () => {
      const iter = new RoaringBitmap32Iterator()
      expect(typeof iter[Symbol.iterator]).toBe('function')
    })

    it('returns this', () => {
      const iter = new RoaringBitmap32Iterator()
      expect(iter[Symbol.iterator]()).toBe(iter)
    })

    it('allows foreach (empty)', () => {
      const iter = new RoaringBitmap32Iterator()
      for (const x of iter) {
        throw new Error(`Should be empty but ${x} found`)
      }
    })

    it('allows foreach (small array)', () => {
      const iter = new RoaringBitmap32Iterator(new RoaringBitmap32([123, 456, 789]))
      const values = []
      for (const x of iter) {
        values.push(x)
      }
      expect(values).toEqual([123, 456, 789])
    })

    it('allows Array.from', () => {
      const iter = new RoaringBitmap32Iterator(new RoaringBitmap32([123, 456, 789]))
      const values = Array.from(iter)
      expect(values).toEqual([123, 456, 789])
    })
  })

  describe('buffer (number)', () => {
    it('iterates, buffer 1, bitmap 0', () => {
      const bitmap = new RoaringBitmap32()
      const iterator = new RoaringBitmap32Iterator(bitmap, 1)
      expect(iterator.next()).toEqual({ value: undefined, done: true })
      expect(iterator.next()).toEqual({ value: undefined, done: true })
    })

    it('iterates, buffer 2, bitmap 0', () => {
      const bitmap = new RoaringBitmap32()
      const iterator = new RoaringBitmap32Iterator(bitmap, 2)
      expect(iterator.next()).toEqual({ value: undefined, done: true })
      expect(iterator.next()).toEqual({ value: undefined, done: true })
    })

    it('iterates, buffer 1, bitmap 1', () => {
      const bitmap = new RoaringBitmap32([5])
      const iterator = new RoaringBitmap32Iterator(bitmap, 1)
      expect(iterator.next()).toEqual({ value: 5, done: false })
      expect(iterator.next()).toEqual({ value: undefined, done: true })
      expect(iterator.next()).toEqual({ value: undefined, done: true })
    })

    it('iterates, buffer 2, bitmap 1', () => {
      const bitmap = new RoaringBitmap32([5])
      const iterator = new RoaringBitmap32Iterator(bitmap, 2)
      expect(iterator.next()).toEqual({ value: 5, done: false })
      expect(iterator.next()).toEqual({ value: undefined, done: true })
      expect(iterator.next()).toEqual({ value: undefined, done: true })
    })

    it('iterates, buffer 1, bitmap 3', () => {
      const bitmap = new RoaringBitmap32([5, 7, 9])
      const iterator = new RoaringBitmap32Iterator(bitmap, 1)
      expect(iterator.next()).toEqual({ value: 5, done: false })
      expect(iterator.next()).toEqual({ value: 7, done: false })
      expect(iterator.next()).toEqual({ value: 9, done: false })
    })

    it('iterates, buffer 2, bitmap 3', () => {
      const bitmap = new RoaringBitmap32([5, 7, 9])
      const iterator = new RoaringBitmap32Iterator(bitmap, 2)
      expect(iterator.next()).toEqual({ value: 5, done: false })
      expect(iterator.next()).toEqual({ value: 7, done: false })
      expect(iterator.next()).toEqual({ value: 9, done: false })
    })
  })

  describe('buffer (Uint32Array)', () => {
    it('iterates, buffer 1, bitmap 0', () => {
      const bitmap = new RoaringBitmap32()
      const buffer = new Uint32Array(1)
      const iterator = new RoaringBitmap32Iterator(bitmap, buffer)
      expect(iterator.next()).toEqual({ value: undefined, done: true })
      expect(iterator.next()).toEqual({ value: undefined, done: true })
      expect(Array.from(buffer)).toEqual([0])
    })

    it('iterates, buffer 2, bitmap 0', () => {
      const bitmap = new RoaringBitmap32()
      const buffer = new Uint32Array(2)
      const iterator = new RoaringBitmap32Iterator(bitmap, buffer)
      expect(iterator.next()).toEqual({ value: undefined, done: true })
      expect(iterator.next()).toEqual({ value: undefined, done: true })
      expect(Array.from(buffer)).toEqual([0, 0])
    })

    it('iterates, buffer 1, bitmap 1', () => {
      const bitmap = new RoaringBitmap32([5])
      const buffer = new Uint32Array(1)
      const iterator = new RoaringBitmap32Iterator(bitmap, buffer)
      expect(buffer[0]).toBe(0)
      expect(iterator.next()).toEqual({ value: 5, done: false })
      expect(buffer[0]).toBe(5)
      expect(iterator.next()).toEqual({ value: undefined, done: true })
      expect(buffer[0]).toBe(5)
      expect(iterator.next()).toEqual({ value: undefined, done: true })
      expect(buffer[0]).toBe(5)
    })

    it('iterates, buffer 2, bitmap 1', () => {
      const bitmap = new RoaringBitmap32([5])
      const buffer = new Uint32Array(2)
      const iterator = new RoaringBitmap32Iterator(bitmap, buffer)
      expect(Array.from(buffer)).toEqual([0, 0])
      expect(iterator.next()).toEqual({ value: 5, done: false })
      expect(Array.from(buffer)).toEqual([5, 0])
      expect(iterator.next()).toEqual({ value: undefined, done: true })
      expect(Array.from(buffer)).toEqual([5, 0])
      expect(iterator.next()).toEqual({ value: undefined, done: true })
      expect(Array.from(buffer)).toEqual([5, 0])
    })

    it('iterates, buffer 1, bitmap 3', () => {
      const bitmap = new RoaringBitmap32([5, 7, 9])
      const buffer = new Uint32Array(1)
      const iterator = new RoaringBitmap32Iterator(bitmap, buffer)
      expect(buffer[0]).toBe(0)
      expect(iterator.next()).toEqual({ value: 5, done: false })
      expect(buffer[0]).toBe(5)
      expect(iterator.next()).toEqual({ value: 7, done: false })
      expect(buffer[0]).toBe(7)
      expect(iterator.next()).toEqual({ value: 9, done: false })
      expect(buffer[0]).toBe(9)
    })

    it('iterates, buffer 2, bitmap 3', () => {
      const bitmap = new RoaringBitmap32([5, 7, 9])
      const buffer = new Uint32Array(2)
      const iterator = new RoaringBitmap32Iterator(bitmap, buffer)
      expect(Array.from(buffer)).toEqual([0, 0])
      expect(iterator.next()).toEqual({ value: 5, done: false })
      expect(Array.from(buffer)).toEqual([5, 7])
      expect(iterator.next()).toEqual({ value: 7, done: false })
      expect(Array.from(buffer)).toEqual([5, 7])
      expect(iterator.next()).toEqual({ value: 9, done: false })
      expect(buffer[0]).toBe(9)
    })
  })

  describe('RoaringBitmap32 iterable', () => {
    it('returns a RoaringBitmap32Iterator', () => {
      const bitmap = new RoaringBitmap32()
      const iterator = bitmap[Symbol.iterator]()
      expect(iterator).toBeInstanceOf(RoaringBitmap32Iterator)
      expect(typeof iterator.next).toBe('function')
    })

    it('has both [Symbol.iterator] and iterator', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.iterator).toBe(bitmap[Symbol.iterator])
    })

    it('returns an empty iterator for an empty bitmap', () => {
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
    it('iterates a non empty bitmap', () => {
      const bitmap = new RoaringBitmap32([0xffffffff, 3])
      const iterator = bitmap[Symbol.iterator]()
      expect(iterator.next()).toEqual({
        done: false,
        value: 3
      })
      expect(iterator.next()).toEqual({
        done: false,
        value: 0xffffffff
      })
      expect(iterator.next()).toEqual({
        done: true,
        value: undefined
      })
    })

    it('throws if the bitmap is changed while iterating', () => {
      const bitmap = new RoaringBitmap32()
      bitmap.addRange(0, 1050)

      function doNothing(_v: any) {}

      try {
        let n = 0
        for (const v of new RoaringBitmap32Iterator(bitmap, 256)) {
          if (n++ === 0) {
            bitmap.add(999999)
          } else {
            doNothing(v)
          }
        }
      } catch (e) {
        expect(e.message).toEqual('RoaringBitmap32 iterator - bitmap changed while iterating')
      }
    })
  })
})
