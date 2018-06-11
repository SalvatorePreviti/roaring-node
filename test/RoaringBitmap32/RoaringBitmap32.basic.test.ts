import RoaringBitmap32 from '../../RoaringBitmap32'

describe('RoaringBitmap32 basic', () => {
  describe('minimum', () => {
    it('returns 4294967295 for an empty bitmap', () => {
      expect(new RoaringBitmap32().minimum()).toBe(4294967295)
    })
    it('returns the minimum value', () => {
      const bitmap = new RoaringBitmap32([5, 3, 1, 2, 9])
      expect(bitmap.minimum()).toBe(1)
    })
  })

  describe('maximum', () => {
    it('returns 0 for an empty bitmap', () => {
      expect(new RoaringBitmap32().maximum()).toBe(0)
    })
    it('returns the maximum value', () => {
      const bitmap = new RoaringBitmap32([5, 3, 1, 2, 9])
      expect(bitmap.maximum()).toBe(9)
    })
  })

  describe('toString', () => {
    it('returns "RoaringBitmap32:0 for an empty bitmap"', () => {
      expect(new RoaringBitmap32().toString()).toBe('RoaringBitmap32:0')
    })

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
    it('returns "[]" for an empty bitmap', () => {
      expect(new RoaringBitmap32().contentToString()).toBe('[]')
    })
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
    it('returns a cloned empty bitmap', () => {
      const bitmap1 = new RoaringBitmap32()
      const bitmap2 = bitmap1.clone()
      expect(bitmap1 !== bitmap2).toBeTruthy()
      expect(bitmap2).toBeInstanceOf(RoaringBitmap32)
      expect(bitmap2.size).toBe(0)
      expect(bitmap2.isEmpty).toBe(true)
    })

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

  describe('select', () => {
    it('returns undefined for invalid values on an empty bitmap', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.select(null as any)).toBe(undefined)
      expect(bitmap.select(undefined as any)).toBe(undefined)
      expect(bitmap.select(-123 as any)).toBe(undefined)
      expect(bitmap.select([123] as any)).toBe(undefined)
      expect(bitmap.select(0)).toBe(undefined)
      expect(bitmap.select(100)).toBe(undefined)
      expect(bitmap.select(0x7fffffff)).toBe(undefined)
      expect(bitmap.select(3)).toBe(undefined)
    })

    it('returns the correct value', () => {
      const bitmap = new RoaringBitmap32([1, 2, 3, 4, 5, 10, 100, 1000, 2000, 3000])
      expect(bitmap.select(0)).toBe(1)
      expect(bitmap.select(1)).toBe(2)
      expect(bitmap.select(2)).toBe(3)
      expect(bitmap.select(3)).toBe(4)
      expect(bitmap.select(4)).toBe(5)
      expect(bitmap.select(5)).toBe(10)
      expect(bitmap.select(6)).toBe(100)
      expect(bitmap.select(7)).toBe(1000)
      expect(bitmap.select(8)).toBe(2000)
      expect(bitmap.select(9)).toBe(3000)
      expect(bitmap.select(10)).toBe(undefined)
    })
  })

  describe('rank', () => {
    it('returns 0 for invalid values', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.rank(null as any)).toBe(0)
      expect(bitmap.rank(undefined as any)).toBe(0)
      expect(bitmap.rank(-123 as any)).toBe(0)
      expect(bitmap.rank([123] as any)).toBe(0)
    })

    it('returns 0 with any value on an empty bitmap', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.rank(0)).toBe(0)
      expect(bitmap.rank(100)).toBe(0)
      expect(bitmap.rank(0x7fffffff)).toBe(0)
      expect(bitmap.rank(0xffffffff)).toBe(0)
    })

    it('returns the correct value', () => {
      const bitmap = new RoaringBitmap32([1, 2, 3, 4, 5, 10, 100, 1000, 2000, 3000])
      expect(bitmap.rank(0)).toBe(0)
      expect(bitmap.rank(1)).toBe(1)
      expect(bitmap.rank(2)).toBe(2)
      expect(bitmap.rank(3)).toBe(3)
      expect(bitmap.rank(4)).toBe(4)
      expect(bitmap.rank(5)).toBe(5)
      expect(bitmap.rank(10)).toBe(6)
      expect(bitmap.rank(100)).toBe(7)
      expect(bitmap.rank(1000)).toBe(8)
      expect(bitmap.rank(2000)).toBe(9)
      expect(bitmap.rank(3000)).toBe(10)
    })
  })

  describe('toUint32Array', () => {
    it('returns an empty Uint32Array for an empty bitmap', () => {
      const a = new RoaringBitmap32().toUint32Array()
      expect(a).toBeInstanceOf(Uint32Array)
      expect(a.length).toBe(0)
    })

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
    it('returns an empty array for an empty bitmap', () => {
      expect(new RoaringBitmap32().toArray()).toEqual([])
    })

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
    it('returns an empty array with an empty bitmap', () => {
      expect(new RoaringBitmap32().toJSON()).toEqual([])
    })

    it('works with JSON.stringify with an empty bitmap', () => {
      const bitmap = new RoaringBitmap32()
      expect(JSON.stringify(bitmap)).toEqual('[]')
    })

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
    it('returns a statistics object for an empty bitmap', () => {
      const bitmap = new RoaringBitmap32()
      const statistics = bitmap.statistics()
      expect(statistics).toEqual({
        containers: 0,
        arrayContainers: 0,
        runContainers: 0,
        bitsetContainers: 0,
        valuesInArrayContainers: 0,
        valuesInRunContainers: 0,
        valuesInBitsetContainers: 0,
        bytesInArrayContainers: 0,
        bytesInRunContainers: 0,
        bytesInBitsetContainers: 0,
        maxValue: 0,
        minValue: 4294967295,
        sumOfAllValues: 0,
        size: 0
      })
    })

    it('returns valid statistics', () => {
      const rb = new RoaringBitmap32()
      rb.addMany([1, 2, 3, 4, 6, 7])
      rb.addMany([999991, 999992, 999993, 999994, 999996, 999997])
      expect(rb.statistics()).toEqual({
        containers: 2,
        arrayContainers: 2,
        runContainers: 0,
        bitsetContainers: 0,
        valuesInArrayContainers: 12,
        valuesInRunContainers: 0,
        valuesInBitsetContainers: 0,
        bytesInArrayContainers: 24,
        bytesInRunContainers: 0,
        bytesInBitsetContainers: 0,
        maxValue: 999997,
        minValue: 1,
        sumOfAllValues: 5999986,
        size: 12
      })
      rb.runOptimize()
      rb.shrinkToFit()
      expect(rb.statistics()).toEqual({
        containers: 2,
        arrayContainers: 0,
        runContainers: 2,
        bitsetContainers: 0,
        valuesInArrayContainers: 0,
        valuesInRunContainers: 12,
        valuesInBitsetContainers: 0,
        bytesInArrayContainers: 0,
        bytesInRunContainers: 20,
        bytesInBitsetContainers: 0,
        maxValue: 999997,
        minValue: 1,
        sumOfAllValues: 5999986,
        size: 12
      })
    })
  })

  describe('removeRunCompression', () => {
    it('does nothing with an empty bitmap', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.removeRunCompression()).toBe(false)
    })
  })

  describe('runOptimize', () => {
    it('does nothing with an empty bitmap', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.runOptimize()).toBe(false)
    })
    it('run optimizes', () => {
      const bitmap = new RoaringBitmap32()
      for (let i = 200; i > 0; --i) {
        bitmap.add(i)
      }
      expect(bitmap.runOptimize()).toBe(true)
      expect(bitmap.statistics().runContainers).toBe(1)
      bitmap.removeRunCompression()
      bitmap.shrinkToFit()
      expect(bitmap.statistics().runContainers).toBe(0)
      expect(bitmap.size).toEqual(200)
    })
  })

  describe('shrinkToFit', () => {
    it('shrinks some memory', () => {
      const bitmap = new RoaringBitmap32()
      expect(bitmap.shrinkToFit()).toBe(44)
      expect(bitmap.shrinkToFit()).toBe(0)
    })
  })

  describe('general tests', () => {
    it('allows adding 900 values', () => {
      const bitmap = new RoaringBitmap32()
      for (let i = 100; i < 1000; ++i) {
        bitmap.add(i)
      }
      expect(bitmap.size).toBe(900)
      expect(bitmap.runOptimize()).toBe(true)
      expect(bitmap.size).toBe(900)
      expect(bitmap.minimum()).toBe(100)
      expect(bitmap.maximum()).toBe(999)
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
