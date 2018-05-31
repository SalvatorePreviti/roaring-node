const roaringNode = require('./build/Release/roaring.node')

if (roaringNode._initTypes) {
  roaringNode._initTypes({
    Buffer,
    Uint32Array
  })
  delete roaringNode._initTypes
}

module.exports = roaringNode
