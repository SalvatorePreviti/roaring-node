const path = require('path')
const fs = require('fs')

let cpu
try {
  const cpuInfoPath = './build/Release/roaring-cpuinfo.node'
  cpu = fs.existsSync(path.join(__dirname, cpuInfoPath)) && require(cpuInfoPath).cpu
} catch (error) {
  // ignore error
}

let roaringNodePath
if (cpu) {
  roaringNodePath = `./build/Release/roaring-${cpu}.node`
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
