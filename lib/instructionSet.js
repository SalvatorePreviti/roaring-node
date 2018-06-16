const os = require('os')
const moduleExists = require('./moduleExists')

const trueValues = {
  true: true,
  True: true,
  TRUE: true,
  1: true
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

let instructionSet = 'PLAIN'

if (os.arch() === 'x64' && (!disableSSE42 || !disableAVX2)) {
  try {
    const cpuinfoNodePath = '../build/Release/cpuinfo.node'
    if (moduleExists(cpuinfoNodePath)) {
      const cpuinfoNode = require(cpuinfoNodePath)
      if (cpuinfoNode.SSE42) {
        if (cpuinfoNode.AVX2 && !parseBool(process.env.ROARING_DISABLE_AVX2)) {
          instructionSet = 'AVX2'
        } else if (!parseBool(process.env.ROARING_DISABLE_SSE42)) {
          instructionSet = 'SSE42'
        }
      }
    }
  } catch (error) {
    // ignore error
  }
}

module.exports = instructionSet
