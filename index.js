'use strict'

const roaring = require('./build/Release/roaring.node')
const { version: packageVersion, roaring_version: CRoaringVersion } = require('./package.json')

module.exports = roaring

const { defineProperty } = Reflect

const { RoaringBitmap32, RoaringBitmap32BufferedIterator } = roaring

class RoaringBitmap32IteratorResult {
  constructor() {
    this.value = undefined
    this.done = true
  }
}

class RoaringBitmap32Iterator {
  constructor(bitmap, buffer = 512) {
    const r = new RoaringBitmap32IteratorResult()
    let t
    let i = 0
    let n = 0

    if (!(bitmap instanceof RoaringBitmap32)) {
      if (bitmap === undefined || bitmap === null) {
        t = null
      } else {
        throw new TypeError('RoaringBitmap32Iterator constructor expects a RoaringBitmap32 instance')
      }
    }

    const init = () => {
      if (typeof buffer === 'number') {
        buffer = new Uint32Array(buffer)
      }
      t = new RoaringBitmap32BufferedIterator(bitmap, buffer)
      n = t.n
      r.done = false
    }

    const m = () => {
      if (t !== null) {
        if (t === undefined) {
          init()
        } else {
          n = t.fill()
        }

        if (n === 0) {
          t = null
          buffer = undefined
          bitmap = undefined
          r.value = undefined
          r.done = true
        } else {
          i = 1
          r.value = buffer[0]
        }
      }
    }

    this.next = () => {
      if (i < n) {
        r.value = buffer[i++]
      } else {
        m()
      }
      return r
    }
  }

  [Symbol.iterator]() {
    return this
  }
}

const roaringBitmap32IteratorProp = {
  value: RoaringBitmap32Iterator,
  writable: false,
  configurable: false,
  enumerable: false
}

defineProperty(RoaringBitmap32Iterator, 'default', roaringBitmap32IteratorProp)
defineProperty(RoaringBitmap32Iterator, 'RoaringBitmap32Iterator', roaringBitmap32IteratorProp)

function iterator() {
  return new RoaringBitmap32Iterator(this)
}

if (!roaring.PackageVersion) {
  const roaringBitmap32Proto = RoaringBitmap32.prototype
  roaringBitmap32Proto[Symbol.iterator] = iterator
  roaringBitmap32Proto.iterator = iterator

  roaring.Promise = Promise
  roaring.RoaringBitmap32Iterator = RoaringBitmap32Iterator
  roaring.PackageVersion = packageVersion
  roaring.CRoaringVersion = CRoaringVersion

  roaring._initTypes({
    Uint32Array
  })
}
