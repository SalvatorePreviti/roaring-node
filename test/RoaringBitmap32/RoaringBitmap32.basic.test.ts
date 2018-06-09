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

  describe('toString', () => {
    it('returns "RoaringBitmap32:1" for a bitmap with 1 element', () => {
      const bitmap = new RoaringBitmap32([1])
      expect(bitmap.toString()).toEqual('RoaringBitmap32:1')
    })

    it('returns "RoaringBitmap32:2" for a bitmap with 2 elements', () => {
      const bitmap = new RoaringBitmap32([1, 2])
      expect(bitmap.toString()).toEqual('RoaringBitmap32:2')
    })
  })

  describe('contentToString', () => {
    it('generates a valid string for 1 value', () => {
      const bitmap = new RoaringBitmap32([1])
      expect(bitmap.contentToString()).toEqual('[1]')
    })
    it('generates a valid string for few values', () => {
      const values = [100, 200, 201, 202, 203, 204, 300, 0x7fffffff, 0xffffffff]
      const bitmap = new RoaringBitmap32(values)
      expect(bitmap.contentToString()).toEqual('[100,200,201,202,203,204,300,2147483647,4294967295]')
    })
    it('supports maxLength correctly', () => {
      const values = [100, 200, 201, 202, 203, 204, 300, 0x7fffffff, 0xffffffff]
      const bitmap = new RoaringBitmap32(values)
      expect(bitmap.contentToString(13)).toEqual('[100,200,201,202...]')
    })
  })

  describe('clone', () => {
    it('returns a cloned bitmap', () => {
      const values = [1, 2, 100, 101, 200, 400, 0x7fffffff, 0xffffffff]
      const bitmap1 = new RoaringBitmap32(values)
      const bitmap2 = bitmap1.clone()
      expect(bitmap1 !== bitmap2).toBeTruthy()
      expect(bitmap2).toBeInstanceOf(RoaringBitmap32)
      expect(bitmap2.size).toBe(values.length)
      expect(bitmap2.isEmpty).toBe(false)
      expect(Array.from(bitmap2.toUint32Array())).toEqual(values)
    })
  })

  describe('toUint32Array', () => {
    it('returns an array with 1 element for 1 element', () => {
      const bitmap = new RoaringBitmap32([1])
      const x = bitmap.toUint32Array()
      expect(x).toBeInstanceOf(Uint32Array)
      expect(x.length).toBe(1)
      expect(Array.from(x)).toEqual([1])
    })

    it('returns an array with multiple elements', () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 0x7fffffff, 0xffffffff])
      const x = bitmap.toUint32Array()
      expect(x).toBeInstanceOf(Uint32Array)
      expect(x.length).toBe(6)
      expect(Array.from(x)).toEqual([1, 2, 10, 30, 0x7fffffff, 0xffffffff])
    })
  })

  describe('toArray', () => {
    it('returns an array with 1 element for 1 element', () => {
      const bitmap = new RoaringBitmap32([1])
      expect(bitmap.toArray()).toEqual([1])
    })

    it('returns an array with multiple elements', () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 0x7fffffff, 0xffffffff])
      expect(bitmap.toArray()).toEqual([1, 2, 10, 30, 0x7fffffff, 0xffffffff])
    })
  })

  describe('toJSON', () => {
    it('returns an array with all the values', () => {
      const bitmap = new RoaringBitmap32([1, 2, 3])
      expect(bitmap.toJSON()).toEqual([1, 2, 3])
    })

    it('works with JSON.stringify', () => {
      const bitmap = new RoaringBitmap32([1, 2, 3])
      expect(JSON.stringify(bitmap)).toEqual('[1,2,3]')
    })
  })
})
