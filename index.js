const cpuInfo = require('./lib/cpuinfo')

let roaringNodePath
if (cpuInfo.AVX2) {
  roaringNodePath = `./build/Release/roaring-avx2.node`
} else if (cpuInfo.SSE42) {
  roaringNodePath = './build/Release/roaring-sse42.node'
} else {
  roaringNodePath = './build/Release/roaring.node'
}

const roaringNode = require(roaringNodePath)

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
