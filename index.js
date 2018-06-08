const roaringNode = require('./build/Release/roaring.node')

roaringNode._initTypes({
  Array,
  Buffer,
  Uint32Array
})

module.exports = roaringNode
