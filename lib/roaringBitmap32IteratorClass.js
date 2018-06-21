class RoaringBitmap32IteratorResult {
  constructor() {
    this.value = undefined
    this.done = true
  }
}

const max = Math.max

function roaringBitmap32IteratorClass(roaring) {
  const RoaringBitmap32BufferedIterator = roaring.RoaringBitmap32BufferedIterator

  function ensureBuffer(bitmap, buffer) {
    if (buffer === undefined) {
      return new Uint32Array(max(bitmap.size || 0, 2048))
    }

    if (typeof buffer === 'number') {
      return new Uint32Array(buffer)
    }
    return buffer
  }

  class RoaringBitmap32Iterator {
    constructor(bitmap, buffer) {
      const state = new RoaringBitmap32IteratorResult()
      let i = 1
      let n = -1
      let it

      function fill() {
        if (n === -1) {
          n = 0
          if (bitmap) {
            buffer = ensureBuffer(bitmap, buffer)
            it = new RoaringBitmap32BufferedIterator(bitmap, buffer)
            n = it.n
            if (n !== 0) {
              state.value = buffer[0]
              state.done = false
            }
          }
        } else if (n === 0) {
          n = -2
          it = undefined
          buffer = undefined
          state.value = undefined
          state.done = true
        }
      }

      function next() {
        if (i < n) {
          state.value = buffer[i++]
        } else {
          fill()
        }
        return state
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
