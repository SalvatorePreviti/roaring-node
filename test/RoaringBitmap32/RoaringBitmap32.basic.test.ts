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

  describe('constructor', () => {
    it('construct with an array of values', () => {
      const values = [1, 2, 3, 6, 7, 8, 20, 44444]
      const bitmap = new RoaringBitmap32(values)
      bitmap.runOptimize()
      bitmap.shrinkToFit()
      expect(bitmap.size).toBe(values.length)
      for (const v of values) {
        expect(bitmap.has(v)).toBe(true)
      }
      expect(bitmap.has(5)).toBe(false)
    })

    it('construct with an UInt32Array', () => {
      const values = [1, 2, 3, 6, 7, 8, 20, 44444]
      const bitmap = new RoaringBitmap32(new Uint32Array(values))
      bitmap.runOptimize()
      bitmap.shrinkToFit()
      expect(bitmap.size).toBe(values.length)
      for (const v of values) {
        expect(bitmap.has(v)).toBe(true)
      }
      expect(bitmap.has(5)).toBe(false)
    })

    it('construct with a RoaringBitmap32', () => {
      const values = [1, 2, 3, 6, 7, 8, 20, 44444]
      const bitmap1 = new RoaringBitmap32(values)
      const bitmap2 = new RoaringBitmap32(bitmap1)
      expect(bitmap2.size).toBe(values.length)
      for (const v of values) {
        expect(bitmap2.has(v)).toBe(true)
      }
      expect(bitmap2.has(5)).toBe(false)
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

  describe('statistics', () => {
    it('returns valid statistics', () => {
      const rb = new RoaringBitmap32()
      rb.addMany([1, 2, 3, 4, 6, 7])
      rb.addMany([999991, 999992, 999993, 999994, 999996, 999997])
      let stats = rb.statistics()
      expect(stats.size).toBe(rb.size)
      expect(stats.containers).toBe(2)
      expect(stats.arrayContainers).toBe(2)
      expect(stats.runContainers).toBe(0)
      expect(stats.bitsetContainers).toBe(0)
      rb.runOptimize()
      rb.shrinkToFit()
      stats = rb.statistics()
      expect(stats.size).toBe(rb.size)
      expect(stats.containers).toBe(2)
      expect(stats.arrayContainers).toBe(0)
      expect(stats.runContainers).toBe(2)
      expect(stats.bitsetContainers).toBe(0)
    })
  })

  describe('simple tests', () => {
    it('allows adding 900 values', () => {
      const bitmap = new RoaringBitmap32()
      for (let i = 100; i < 1000; ++i) {
        bitmap.add(i)
      }
      expect(bitmap.size).toBe(900)
      expect(bitmap.runOptimize()).toBe(true)
      expect(bitmap.size).toBe(900)
    })

    it('works with some "fancy" operations', () => {
      const rb1 = new RoaringBitmap32()
      rb1.add(1)
      rb1.add(2)
      rb1.add(3)
      rb1.add(4)
      rb1.add(5)
      rb1.add(100)
      rb1.add(1000)
      rb1.runOptimize()

      const rb2 = new RoaringBitmap32([3, 4, 1000])
      rb2.runOptimize()
      const rb3 = new RoaringBitmap32()

      expect(rb1.size).toBe(7)
      expect(rb1.has(1)).toBe(true)
      expect(rb1.has(3)).toBe(true)

      expect(rb1.toString()).toEqual('RoaringBitmap32:7')
      expect(rb1.contentToString()).toEqual('[1,2,3,4,5,100,1000]')

      rb1.andInPlace(rb2)

      expect(rb1.has(1)).toBe(false)
      expect(rb1.has(3)).toBe(true)

      expect(rb1.toString()).toEqual('RoaringBitmap32:3')
      expect(rb1.contentToString()).toEqual('[3,4,1000]')

      rb3.add(5)
      rb3.orInPlace(rb1)

      expect(Array.from(rb3)).toEqual([3, 4, 5, 1000])
      expect(rb3.toArray()).toEqual([3, 4, 5, 1000])
      expect(Array.from(rb3.toUint32Array())).toEqual([3, 4, 5, 1000])

      const rb4 = RoaringBitmap32.orMany(rb1, rb2, rb3)
      expect(rb4.toArray()).toEqual([3, 4, 5, 1000])

      const serialized = rb1.serialize()
      const rb5 = RoaringBitmap32.deserialize(serialized)
      expect(rb5.isEqual(rb1)).toBe(true)
      expect(rb1.isEqual(rb5)).toBe(true)
      expect(rb5.toArray()).toEqual(rb1.toArray())
    })
  })
})
