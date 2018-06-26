import RoaringBitmap32 = require('../../RoaringBitmap32')

describe('RoaringBitmap32 operations', () => {
  describe('add, has', () => {
    it('works with multiple values', () => {
      const values = [100, 120, 130, 140, 150, 99, 1, 13, 64]
      const bitmap = new RoaringBitmap32()
      for (const value of values) {
        expect(bitmap.has(value)).toBe(false)
        expect(bitmap.add(value)).toBe(bitmap)
      }
      for (const value of values) {
        expect(bitmap.has(value)).toBe(true)
      }
      expect(Array.from(bitmap)).toEqual(values.slice().sort((a, b) => a - b))
    })

    it('accepts 32 bit big numbers', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.has(0x7fffffff)).toBe(false)
      expect(bitmap.has(0xfffffffe)).toBe(false)
      expect(bitmap.has(0xffffffff)).toBe(false)
      bitmap.add(0x7fffffff)
      bitmap.add(0xfffffffe)
      bitmap.add(0xffffffff)
      expect(bitmap.has(0x7fffffff)).toBe(true)
      expect(bitmap.has(0xfffffffe)).toBe(true)
      expect(bitmap.has(0xffffffff)).toBe(true)
      expect(Array.from(bitmap)).toEqual([2147483647, 4294967294, 4294967295])
    })

    it('returns false with some random ids on an empty bitmap', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.has(0)).toBe(false)
      expect(bitmap.has(12340)).toBe(false)
      expect(bitmap.has(0xffffffff)).toBe(false)
      expect(bitmap.has(0xffffffff >>> 0)).toBe(false)
    })

    it('returns false with invalid values on an empty bitmap', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.has(0.5)).toBe(false)
      expect(bitmap.has(-12340)).toBe(false)
      expect(bitmap.has('xxxx' as any)).toBe(false)
      expect(bitmap.has(null as any)).toBe(false)
      expect(bitmap.has(undefined as any)).toBe(false)
      expect(bitmap.has(bitmap as any)).toBe(false)
    })
  })

  describe('addMany', () => {
    it('works with an array', () => {
      const values = [100, 120, 130, 140, 150, 99, 1, 13, 64]
      const sortedValues = values.slice().sort((a, b) => a - b)
      const bitmap = new RoaringBitmap32()
      bitmap.addMany(values)
      for (const value of values) {
        expect(bitmap.has(value)).toBe(true)
      }

      let i = 0
      for (const value of bitmap) {
        expect(value).toBe(sortedValues[i++])
      }

      expect(i).toBe(sortedValues.length)
      expect(bitmap.size).toBe(values.length)
      expect(Array.from(bitmap)).toEqual(sortedValues)
    })

    it('works with an Uint32Array', () => {
      const values = [100, 120, 130, 140, 150, 99, 1, 13, 64]
      const bitmap = new RoaringBitmap32()
      bitmap.addMany(new Uint32Array(values))
      for (const value of values) {
        expect(bitmap.has(value)).toBe(true)
      }
      const valuesSet = new Set(values)
      for (const value of bitmap) {
        expect(valuesSet.delete(value)).toBe(true)
      }
      expect(valuesSet.size).toBe(0)
      expect(Array.from(bitmap)).toEqual(values.slice().sort((a, b) => a - b))
    })
  })

  describe('clear', () => {
    it('clear does nothing on an empty bitmap', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.clear()).toBe(false)
      expect(bitmap.size).toBe(0)
      expect(bitmap.isEmpty).toBe(true)
    })

    it('removes all items', () => {
      const bitmap = new RoaringBitmap32([1, 2, 0x7fffffff, 3, 0xffffffff])
      bitmap.clear()
      expect(bitmap.isEmpty).toBe(true)
      expect(bitmap.size).toBe(0)
      expect(bitmap.toArray()).toEqual([])
    })
  })

  describe('remove', () => {
    it('remove does nothing on an empty bitmap', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.remove(19)).toBe(undefined)
      expect(bitmap.size).toBe(0)
    })

    it('remove one item', () => {
      const bitmap = new RoaringBitmap32([1, 5, 6])
      expect(bitmap.remove(5)).toBe(undefined)
      expect(bitmap.toArray()).toEqual([1, 6])
      expect(bitmap.remove(5)).toBe(undefined)
      expect(bitmap.toArray()).toEqual([1, 6])
    })
  })

  describe('delete', () => {
    it('delete does nothing on an empty bitmap', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.delete(19)).toBe(false)
      expect(bitmap.size).toBe(0)
    })

    it('remove one item', () => {
      const bitmap = new RoaringBitmap32([1, 5, 6])
      expect(bitmap.delete(5)).toBe(true)
      expect(bitmap.toArray()).toEqual([1, 6])
      expect(bitmap.delete(5)).toBe(false)
      expect(bitmap.toArray()).toEqual([1, 6])
    })
  })

  describe('removeMany', () => {
    it('removeMany does nothing on an empty bitmap', () => {
      const bitmap = new RoaringBitmap32()
      bitmap.removeMany([123])
      expect(bitmap.size).toBe(0)
    })

    it('removeMany removes one item', () => {
      const bitmap = new RoaringBitmap32([1, 2, 123, 3])
      bitmap.removeMany([123])
      expect(bitmap.toArray()).toEqual([1, 2, 3])
    })

    it('removeMany removes multiple items', () => {
      const bitmap = new RoaringBitmap32([1, 2, 123, 3, 4, 5, 6])
      bitmap.removeMany([123, 5, 6])
      expect(bitmap.toArray()).toEqual([1, 2, 3, 4])
      bitmap.removeMany([123, 5, 6])
      expect(bitmap.toArray()).toEqual([1, 2, 3, 4])
    })

    it('removeMany removes multiple items (Uint32Array)', () => {
      const bitmap = new RoaringBitmap32([1, 2, 123, 3, 4, 5, 6])
      bitmap.removeMany(new Uint32Array([123, 5, 6]))
      expect(bitmap.toArray()).toEqual([1, 2, 3, 4])
    })

    it('removeMany removes multiple items (RoaringBitmap32)', () => {
      const bitmap = new RoaringBitmap32([1, 2, 123, 3, 4, 5, 6])
      bitmap.removeMany(new RoaringBitmap32([123, 5, 6]))
      expect(bitmap.toArray()).toEqual([1, 2, 3, 4])
    })
  })

  describe('andInPlace', () => {
    it('does nothing with an empty bitmaps', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32()
      a.andInPlace(b)
      expect(a.isEmpty).toBe(true)
    })

    it('does nothing with an empty bitmap and an empty array', () => {
      const a = new RoaringBitmap32()
      a.andInPlace([])
      expect(a.isEmpty).toBe(true)
    })

    it('does nothing with an empty bitmap and a non empty array', () => {
      const a = new RoaringBitmap32()
      a.andInPlace([1, 2, 3])
      expect(a.isEmpty).toBe(true)
    })

    it('ands two RoaringBitmap32', () => {
      const a = new RoaringBitmap32([1, 2, 3, 4])
      a.andInPlace(new RoaringBitmap32([1, 4]))
      expect(a.toArray()).toEqual([1, 4])
    })

    it('ands one RoaringBitmap32 and one array', () => {
      const a = new RoaringBitmap32([1, 2, 3, 4])
      a.andInPlace([1, 4])
      expect(a.toArray()).toEqual([1, 4])
    })

    it('ands one RoaringBitmap32 and one UInt32Array', () => {
      const a = new RoaringBitmap32([1, 2, 3, 4])
      a.andInPlace(new Uint32Array([1, 4]))
      expect(a.toArray()).toEqual([1, 4])
    })

    it('ands one RoaringBitmap32 and one Int32Array', () => {
      const a = new RoaringBitmap32([1, 2, 3, 4])
      a.andInPlace(new Int32Array([1, 4]))
      expect(a.toArray()).toEqual([1, 4])
    })
  })

  describe('andNotInPlace', () => {
    it('does nothing with two empty bitmaps', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32()
      a.andNotInPlace(b)
      expect(a.isEmpty).toBe(true)
    })

    it('does nothing with an empty bitmap and empty array', () => {
      const a = new RoaringBitmap32()
      a.andNotInPlace([])
      expect(a.isEmpty).toBe(true)
    })

    it('does nothing with an empty bitmap and a non empty array', () => {
      const a = new RoaringBitmap32()
      a.andNotInPlace([1, 2, 3])
      expect(a.isEmpty).toBe(true)
    })

    it('and not two bitmaps', () => {
      const a = new RoaringBitmap32([1, 2, 3, 4])
      a.andNotInPlace(new RoaringBitmap32([1, 2]))
      expect(a.toArray()).toEqual([3, 4])
    })

    it('and not a bitmap and an array', () => {
      const a = new RoaringBitmap32([1, 2, 3, 4])
      a.andNotInPlace([1, 2])
      expect(a.toArray()).toEqual([3, 4])
    })

    it('and not a bitmap and an UInt32Array', () => {
      const a = new RoaringBitmap32([1, 2, 3, 4])
      a.andNotInPlace(new Uint32Array([1, 2]))
      expect(a.toArray()).toEqual([3, 4])
    })

    it('and not a bitmap and an Int32Array', () => {
      const a = new RoaringBitmap32([1, 2, 3, 4])
      a.andNotInPlace(new Int32Array([1, 2]))
      expect(a.toArray()).toEqual([3, 4])
    })
  })

  describe('orInPlace', () => {
    it('does nothing with two empty bitmaps', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32()
      a.orInPlace(b)
      expect(a.isEmpty).toBe(true)
    })

    it('does nothing with an empty bitmap and one empty array', () => {
      const a = new RoaringBitmap32()
      a.orInPlace([])
      expect(a.isEmpty).toBe(true)
    })

    it('adds values to an empty bitmap (array)', () => {
      const a = new RoaringBitmap32()
      a.orInPlace([1, 2, 3])
      expect(a.toArray()).toEqual([1, 2, 3])
    })

    it('adds values to an empty bitmap (Uint32Array)', () => {
      const a = new RoaringBitmap32()
      a.orInPlace(new Uint32Array([1, 2, 3]))
      expect(a.toArray()).toEqual([1, 2, 3])
    })

    it('adds values to an empty bitmap (Int32Array)', () => {
      const a = new RoaringBitmap32()
      a.orInPlace(new Int32Array([1, 2, 3]))
      expect(a.toArray()).toEqual([1, 2, 3])
    })

    it('adds values to an empty bitmap (RoaringBitmap32)', () => {
      const a = new RoaringBitmap32()
      a.orInPlace(new RoaringBitmap32([1, 2, 3]))
      expect(a.toArray()).toEqual([1, 2, 3])
    })

    it('ors two bitmaps', () => {
      const a = new RoaringBitmap32([1, 3, 5])
      a.orInPlace(new RoaringBitmap32([2, 4, 5]))
      expect(a.toArray()).toEqual([1, 2, 3, 4, 5])
    })

    it('ors a bitmap and an array', () => {
      const a = new RoaringBitmap32([1, 3, 5])
      a.orInPlace([2, 4, 5])
      expect(a.toArray()).toEqual([1, 2, 3, 4, 5])
    })

    it('ors a bitmap and an Uint32Array', () => {
      const a = new RoaringBitmap32([1, 3, 5])
      a.orInPlace(new Uint32Array([2, 4, 5]))
      expect(a.toArray()).toEqual([1, 2, 3, 4, 5])
    })

    it('ors a bitmap and an Int32Array', () => {
      const a = new RoaringBitmap32([1, 3, 5])
      a.orInPlace(new Int32Array([2, 4, 5]))
      expect(a.toArray()).toEqual([1, 2, 3, 4, 5])
    })
  })

  describe('xorInPlace', () => {
    it('does nothing with two empty bitmaps', () => {
      const a = new RoaringBitmap32()
      const b = new RoaringBitmap32()
      a.xorInPlace(b)
      expect(a.isEmpty).toBe(true)
    })

    it('does nothing with an empty bitmap and an empty array', () => {
      const a = new RoaringBitmap32()
      a.xorInPlace([])
      expect(a.isEmpty).toBe(true)
    })

    it('adds values to an empty bitmap (array)', () => {
      const a = new RoaringBitmap32()
      a.xorInPlace([1, 2, 3])
      expect(a.toArray()).toEqual([1, 2, 3])
    })

    it('adds values to an empty bitmap (Uint32Array)', () => {
      const a = new RoaringBitmap32()
      a.xorInPlace(new Uint32Array([1, 2, 3]))
      expect(a.toArray()).toEqual([1, 2, 3])
    })

    it('adds values to an empty bitmap (Int32Array)', () => {
      const a = new RoaringBitmap32()
      a.xorInPlace(new Int32Array([1, 2, 3]))
      expect(a.toArray()).toEqual([1, 2, 3])
    })

    it('xors (array)', () => {
      const a = new RoaringBitmap32([1, 2, 3])
      a.xorInPlace([1, 2, 3, 4])
      expect(a.toArray()).toEqual([4])
    })

    it('xors (Uint32Array)', () => {
      const a = new RoaringBitmap32([1, 2, 3])
      a.xorInPlace(new Uint32Array([1, 2, 3, 4]))
      expect(a.toArray()).toEqual([4])
    })

    it('xors (Int32Array)', () => {
      const a = new RoaringBitmap32([1, 2, 3])
      a.xorInPlace(new Int32Array([1, 2, 3, 4]))
      expect(a.toArray()).toEqual([4])
    })

    it('xors (bitmap)', () => {
      const a = new RoaringBitmap32([1, 2, 4, 3])
      a.xorInPlace(new RoaringBitmap32([1, 2, 3]))
      expect(a.toArray()).toEqual([4])
    })
  })

  describe('copyFrom', () => {
    describe('to an empty bitmap', () => {
      it('does nothing when copying (undefined)', () => {
        const bitmap = new RoaringBitmap32()
        bitmap.copyFrom(undefined)
        expect(bitmap.size).toBe(0)
        expect(bitmap.isEmpty).toBe(true)
      })

      it('does nothing when copying (null)', () => {
        const bitmap = new RoaringBitmap32()
        bitmap.copyFrom(null)
        expect(bitmap.size).toBe(0)
        expect(bitmap.isEmpty).toBe(true)
      })

      it('does nothing when copying self (empty)', () => {
        const bitmap = new RoaringBitmap32()
        bitmap.copyFrom(bitmap)
        expect(bitmap.size).toBe(0)
        expect(bitmap.isEmpty).toBe(true)
      })

      it('does nothing when copying an empty array', () => {
        const bitmap = new RoaringBitmap32()
        bitmap.copyFrom([])
        expect(bitmap.size).toBe(0)
        expect(bitmap.isEmpty).toBe(true)
      })

      it('does nothing when copying an empty Uint32Array', () => {
        const bitmap = new RoaringBitmap32()
        bitmap.copyFrom(new Uint32Array([]))
        expect(bitmap.size).toBe(0)
        expect(bitmap.isEmpty).toBe(true)
      })

      it('does nothing when copying an empty Int32Array', () => {
        const bitmap = new RoaringBitmap32()
        bitmap.copyFrom(new Int32Array([]))
        expect(bitmap.size).toBe(0)
        expect(bitmap.isEmpty).toBe(true)
      })

      it('does nothing when copying an empty RoaringBitmap32', () => {
        const bitmap = new RoaringBitmap32()
        bitmap.copyFrom(new RoaringBitmap32())
        expect(bitmap.size).toBe(0)
        expect(bitmap.isEmpty).toBe(true)
      })
    })

    describe('to a bitmap with content', () => {
      it('clears if passed undefined', () => {
        const bitmap = new RoaringBitmap32([1, 2, 3])
        bitmap.copyFrom(undefined)
        expect(bitmap.size).toBe(0)
        expect(bitmap.isEmpty).toBe(true)
      })

      it('clears if passed null', () => {
        const bitmap = new RoaringBitmap32([1, 2, 3])
        bitmap.copyFrom(null)
        expect(bitmap.size).toBe(0)
        expect(bitmap.isEmpty).toBe(true)
      })

      it('clears if passed an empty array', () => {
        const bitmap = new RoaringBitmap32([1, 2, 3])
        bitmap.copyFrom([])
        expect(bitmap.size).toBe(0)
        expect(bitmap.isEmpty).toBe(true)
      })

      it('clears if passed an empty RoaringBitmap32', () => {
        const bitmap = new RoaringBitmap32([1, 2, 3])
        bitmap.copyFrom(new RoaringBitmap32())
        expect(bitmap.size).toBe(0)
        expect(bitmap.isEmpty).toBe(true)
      })

      it('replace with the given values (array)', () => {
        const bitmap = new RoaringBitmap32([1, 2, 3])
        bitmap.copyFrom([4, 6, 8, 10])
        expect(bitmap.toArray()).toEqual([4, 6, 8, 10])
      })

      it('replace with the given values (Uint32Array)', () => {
        const bitmap = new RoaringBitmap32([1, 2, 3])
        bitmap.copyFrom(new Uint32Array([4, 6, 8, 10]))
        expect(bitmap.toArray()).toEqual([4, 6, 8, 10])
      })

      it('replace with the given values (Int32Array)', () => {
        const bitmap = new RoaringBitmap32([1, 2, 3])
        bitmap.copyFrom(new Int32Array([4, 6, 8, 10]))
        expect(bitmap.toArray()).toEqual([4, 6, 8, 10])
      })

      it('replace with the given values (bitmap)', () => {
        const bitmap = new RoaringBitmap32([1, 2, 3])
        bitmap.copyFrom(new RoaringBitmap32([4, 6, 8, 10]))
        expect(bitmap.toArray()).toEqual([4, 6, 8, 10])
      })
    })
  })
})
