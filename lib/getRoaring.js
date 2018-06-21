const instructionSet = require('./instructionSet')
const moduleExists = require('./moduleExists')

const roaringNodePaths = {
  AVX2: '../build/Release/roaring-avx2.node',
  SSE42: '../build/Release/roaring-sse42.node',
  PLAIN: '../build/Release/roaring.node'
}

const cache = {
  AVX2: undefined,
  SSE42: undefined,
  PLAIN: undefined
}

let packageVersion

function getPackageVersion() {
  if (packageVersion === undefined) {
    packageVersion = require('../package.json').version
  }
  return packageVersion
}

class RoaringBitmap32IteratorResult {
  constructor() {
    this.value = undefined
    this.done = true
  }
}

function createIteratorClass(roaringNode) {
  const RoaringBitmap32BufferedIterator = roaringNode.RoaringBitmap32BufferedIterator

  class RoaringBitmap32Iterator {
    constructor(bitmap, buffer) {
      const state = new RoaringBitmap32IteratorResult()
      let bufferedIterator
      let bufferLength = -1
      let index = 1

      function advance() {
        if (bufferLength === -1) {
          if (!(buffer instanceof Uint32Array)) {
            buffer = new Uint32Array(typeof buffer === 'number' ? buffer : Math.max(bitmap.size, 2048))
          }
          bufferedIterator = new RoaringBitmap32BufferedIterator(bitmap, buffer)
          bufferLength = bufferedIterator.fill()
          state.value = buffer[0]
          state.done = false
        }

        if (bufferLength === 0) {
          bufferLength = -2
          buffer = undefined
          bufferedIterator = undefined
          state.value = undefined
          state.done = true
        }
      }

      function next() {
        if (index < bufferLength) {
          state.value = buffer[index++]
        } else {
          advance()
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

function loadRoaring(roaringNodePath) {
  const roaringNode = require(roaringNodePath)

  roaringNode._initTypes({
    Set,
    Array,
    Buffer,
    Uint32Array
  })

  if (!roaringNode.hasOwnProperty('PackageVersion')) {
    Object.defineProperty(roaringNode, 'PackageVersion', {
      get: getPackageVersion,
      enumerable: true,
      configurable: false
    })
  }

  const RoaringBitmap32Iterator = createIteratorClass(roaringNode)

  roaringNode.RoaringBitmap32.prototype[Symbol.iterator] = () => {
    return new RoaringBitmap32Iterator(this)
  }

  roaringNode.RoaringBitmap32Iterator = RoaringBitmap32Iterator

  return roaringNode
}

function getByInstructionSet(set) {
  const cached = cache[set]
  if (typeof cached === 'object') {
    return cached === null ? undefined : cached
  }

  const roaringNodePath = roaringNodePaths[set]
  if (typeof roaringNodePath !== 'string') {
    throw new TypeError(`Unknown instruction set: ${set}`)
  }

  if (
    (set === 'AVX2' && instructionSet !== 'AVX2') ||
    (set === 'SSE42' && instructionSet === 'PLAIN') ||
    (instructionSet !== 'PLAIN' && !moduleExists(roaringNodePath))
  ) {
    cache[set] = null
    return undefined
  }

  const roaring = loadRoaring(roaringNodePath)
  cache[set] = roaring
  return roaring
}

function getRoaring(instructionSetToUse) {
  if (instructionSetToUse !== undefined) {
    return getByInstructionSet(instructionSetToUse)
  }
  const cached = cache[instructionSet]
  if (typeof cached === 'object') {
    return cached === null ? undefined : cached
  }
  if (instructionSet === 'AVX2') {
    return getByInstructionSet('AVX2') || getByInstructionSet('SSE42') || getByInstructionSet('PLAIN')
  }
  if (instructionSet === 'SSE42') {
    return getByInstructionSet('SSE42') || getByInstructionSet('PLAIN')
  }
  return getByInstructionSet('PLAIN')
}

module.exports = getRoaring
