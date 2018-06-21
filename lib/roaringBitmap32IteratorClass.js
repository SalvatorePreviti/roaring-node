function doNothing() {}

class RoaringBitmap32IteratorResult {
  constructor() {
    this.value = undefined
    this.done = true
  }
}

function roaringBitmap32IteratorClass(roaring) {
  const RoaringBitmap32 = roaring.RoaringBitmap32
  const RoaringBitmap32BufferedIterator = roaring.RoaringBitmap32BufferedIterator

  function throwInvalidBitmap() {
    throw new TypeError('RoaringBitmap32Iterator constructor expects a RoaringBitmap32 instance')
  }

  class RoaringBitmap32Iterator {
    constructor(bitmap, buffer) {
      const r = new RoaringBitmap32IteratorResult()
      let i = 0
      let n = 0
      let t

      let m = () => {
        if (t === undefined) {
          if (buffer === undefined) {
            buffer = new Uint32Array(1024)
          } else if (typeof buffer === 'number') {
            buffer = new Uint32Array(buffer)
          }
          t = new RoaringBitmap32BufferedIterator(bitmap, buffer)
          n = t.n
          r.done = false
        } else {
          n = t.fill()
        }

        if (n === 0) {
          m = doNothing
          t = undefined
          buffer = undefined
          bitmap = undefined
          r.value = undefined
          r.done = true
        } else {
          i = 1
          r.value = buffer[0]
        }
      }

      function next() {
        if (i < n) {
          r.value = buffer[i++]
        } else {
          m()
        }
        return r
      }

      Object.defineProperty(this, 'next', {
        value: next,
        writable: false,
        configurable: false,
        enumerable: false
      })

      if (!(bitmap instanceof RoaringBitmap32)) {
        if (bitmap === undefined || bitmap === null) {
          m = doNothing
        } else {
          throwInvalidBitmap()
        }
      }
    }

    [Symbol.iterator]() {
      return this
    }
  }

  Object.defineProperty(RoaringBitmap32Iterator, 'default', {
    value: RoaringBitmap32Iterator,
    writable: false,
    configurable: false,
    enumerable: false
  })

  return RoaringBitmap32Iterator
}

module.exports = roaringBitmap32IteratorClass
