const roaringBitmap32IteratorClass = require('./roaringBitmap32IteratorClass')

let _roaring

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

  if (!Object.prototype.hasOwnProperty.call(roaringNode, 'PackageVersion')) {
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

function getRoaring() {
  return _roaring || (_roaring = loadRoaring('../build/Release/roaring.node'))
}

module.exports = getRoaring
