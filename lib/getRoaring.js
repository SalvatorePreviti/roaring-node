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
