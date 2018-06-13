const path = require('path')
const fs = require('fs')
const os = require('os')

const cpuinfo = {
  SSE42: false,
  AVX2: false
}

const disableSSE42 = !!process.env.ROARING_DISABLE_SSE42
const disableAVX2 = !!process.env.ROARING_DISABLE_AVX2

if (os.arch() === 'x64' && (!disableSSE42 || !disableAVX2)) {
  try {
    const cpuinfoNodePath = '../build/Release/cpuinfo.node'
    if (fs.existsSync(path.join(__dirname, cpuinfoNodePath))) {
      const cpuinfoNode = require(cpuinfoNodePath)
      cpuinfo.SSE42 = !!cpuinfoNode.SSE42 && !disableSSE42
      cpuinfo.AVX2 = !!cpuinfoNode.AVX2 && !disableAVX2
    }
  } catch (error) {
    // ignore error
  }
}

module.exports = cpuinfo
