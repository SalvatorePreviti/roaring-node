const path = require('path')
const fs = require('fs')
const os = require('os')

const trueValues = {
  true: true,
  True: true,
  TRUE: true,
  1: true
}

const cpuinfo = {
  SSE42: false,
  AVX2: false
}

function parseBool(value) {
  if (value === null || value === undefined) {
    return false
  }
  if (typeof value === 'boolean') {
    return value
  }
  if (typeof value === 'number' && !Number.isNaN(value)) {
    return value !== 0
  }
  if (typeof value === 'string') {
    if (trueValues[value]) {
      return true
    }
    const p = Number.parseInt(value, 10)
    if (!Number.isNaN(p)) {
      return Number.isFinite(p) ? p !== 0 : false
    }
    return false
  }
  return !!value
}

const disableSSE42 = parseBool(process.env.ROARING_DISABLE_SSE42)
const disableAVX2 = parseBool(process.env.ROARING_DISABLE_AVX2)

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
