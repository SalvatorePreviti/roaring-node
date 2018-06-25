const roaringBitmap32IteratorClass = require('./roaringBitmap32IteratorClass')
const instructionSet = require('./instructionSet')
const moduleExists = require('./moduleExists')
const os = require('os')

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

function loadRoaring(roaringNodePath) {
  const roaringNode = require(roaringNodePath)

  roaringNode._initTypes({
    Set,
    Array,
    Buffer,
    Uint32Array
  })

  roaringNode.Promise = Promise

  if (!roaringNode.hasOwnProperty('PackageVersion')) {
    Object.defineProperty(roaringNode, 'PackageVersion', {
      get: getPackageVersion,
      enumerable: true,
      configurable: false
    })
  }

  const RoaringBitmap32 = roaringNode.RoaringBitmap32
  const RoaringBitmap32Iterator = roaringBitmap32IteratorClass(roaringNode)
  roaringNode.RoaringBitmap32Iterator = RoaringBitmap32Iterator

  function iterator() {
    return new RoaringBitmap32Iterator(this)
  }

  RoaringBitmap32.prototype[Symbol.iterator] = iterator
  RoaringBitmap32.prototype.iterator = iterator

  return roaringNode
}

function getRoaringByInstructionSet(set) {
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
    return getRoaringByInstructionSet(instructionSetToUse)
  }
  const cached = cache[instructionSet]
  if (typeof cached === 'object') {
    return cached === null ? undefined : cached
  }
  if (instructionSet === 'AVX2') {
    return getRoaringByInstructionSet('AVX2') || getRoaringByInstructionSet('SSE42') || getRoaringByInstructionSet('PLAIN')
  }
  if (instructionSet === 'SSE42') {
    return getRoaringByInstructionSet('SSE42') || getRoaringByInstructionSet('PLAIN')
  }
  return getRoaringByInstructionSet('PLAIN')
}

module.exports = getRoaring
