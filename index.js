const roaringNode = require('./build/Release/roaring.node')

roaringNode._initTypes({
  Array,
  Buffer,
  Uint32Array
})

if (!roaringNode.hasOwnProperty('PackageVersion')) {
  Object.defineProperty(roaringNode, 'PackageVersion', {
    get() {
      return require('./package.json').version
    },
    enumerable: true,
    configurable: false
  })
}

module.exports = roaringNode
