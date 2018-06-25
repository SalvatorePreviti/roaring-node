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

let instructionSet = 'PLAIN'

if (!parseBool(process.env.ROARING_DISABLE_SSE42) && os.arch() === 'x64') {
  try {
    const cpuinfoNodePath = '../build/Release/cpuinfo.node'
    if (moduleExists(cpuinfoNodePath)) {
      const cpuinfoNode = require(cpuinfoNodePath)
      if (cpuinfoNode && cpuinfoNode.SSE42) {
        const disableAVX2 = parseBool(process.env.ROARING_DISABLE_AVX2)
        instructionSet = !disableAVX2 && cpuinfoNode.AVX2 ? 'AVX2' : 'SSE42'
      }
    }
  } catch (error) {
    // ignore error
  }
}

module.exports = instructionSet
