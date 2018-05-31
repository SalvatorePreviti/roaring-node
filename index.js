const roaringNode = require('./build/Release/roaring.node')

if (roaringNode._initTypes) {
  roaringNode._initTypes({
    Uint8Array,
    Uint32Array
  })
  delete roaringNode._initTypes
}

module.exports = roaringNode
