const roaringNode = require('./build/Release/roaring.node')

roaringNode._initTypes({
  Buffer,
  Uint32Array
})

module.exports = roaringNode
