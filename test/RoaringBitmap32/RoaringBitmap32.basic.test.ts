import RoaringBitmap32 from '../../RoaringBitmap32'

describe('RoaringBitmap32 basic', () => {
  describe('add', () => {
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
      expect(Array.from(bitmap)).toEqual(values.slice().sort((a, b) => a - b))
    })
  })

  describe('constructor', () => {
    it('works with an array', () => {
      const values = [100, 120, 130, 140, 150, 99, 1, 13, 64]
      const bitmap = new RoaringBitmap32(values)
      for (const value of values) {
        expect(bitmap.has(value)).toBe(true)
      }
      expect(Array.from(bitmap)).toEqual(values.slice().sort((a, b) => a - b))
    })

    it('works with an Uint32Array', () => {
      const values = [100, 120, 130, 140, 150, 99, 1, 13, 64]
      const bitmap = new RoaringBitmap32(new Uint32Array(values))
      for (const value of values) {
        expect(bitmap.has(value)).toBe(true)
      }
      expect(Array.from(bitmap)).toEqual(values.slice().sort((a, b) => a - b))
    })
  })
})
