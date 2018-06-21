class RoaringBitmap32IteratorResult {
  constructor() {
    this.value = undefined
    this.done = true
  }
}

function roaringBitmap32IteratorClass(roaring) {
  const RoaringBitmap32 = roaring.RoaringBitmap32
  const RoaringBitmap32BufferedIterator = roaring.RoaringBitmap32BufferedIterator

  function ensureBuffer(bitmap, buffer) {
    if (buffer === undefined) {
      buffer = new Uint32Array(1024)
    } else if (typeof buffer === 'number') {
      buffer = new Uint32Array(buffer)
    }
    return buffer
  }

  function throwInvalidBitmap() {
    throw new TypeError('RoaringBitmap32Iterator constructor expects a RoaringBitmap32 instance')
  }

  class RoaringBitmap32Iterator {
    constructor(bitmap, buffer) {
      const r = new RoaringBitmap32IteratorResult()
      let i = 0
      let n = 0
      let it

      if (!(bitmap instanceof RoaringBitmap32)) {
        if (bitmap === undefined || bitmap === null) {
          it = null
        } else {
          throwInvalidBitmap()
        }
      }

      function m() {
        if (it === undefined) {
          n = 0
          buffer = ensureBuffer(bitmap, buffer)
          it = new RoaringBitmap32BufferedIterator(bitmap, buffer)
          n = it.n
          r.done = false
        } else {
          n = it.fill()
        }

        if (n === 0) {
          it = null
          buffer = undefined
          r.value = undefined
          r.done = true
        } else {
          r.value = buffer[0]
          i = 1
        }
      }

      function next() {
        if (i < n) {
          r.value = buffer[i++]
        } else if (it !== null) {
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
