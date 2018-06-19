#!/usr/bin/env node

const chalk = require('chalk').default
const os = require('os')
const instructionSet = require('../lib/instructionSet')

function getSystemInfo() {
  const cpus = os.cpus()
  let physicalCpuCount

  try {
    physicalCpuCount = require('physical-cpu-count')
  } catch (e) {
    // Ignore error
  }
  if (!physicalCpuCount) {
    physicalCpuCount = Math.max(1, cpus.length / 2)
  }

  const systemInfo = {
    physicalCpuCount,
    instructionSet
  }

  systemInfo.print = function print() {
    console.log()
    console.log(
      chalk.whiteBright('Platform'),
      ':',
      chalk.cyanBright(os.type()),
      chalk.greenBright(os.release()),
      chalk.yellowBright(process.arch)
    )
    console.log(
      chalk.whiteBright('CPU     '),
      ':',
      chalk.cyanBright(cpus[0].model),
      instructionSet !== 'PLAIN' ? chalk.greenBright(instructionSet) : ''
    )
    console.log(
      chalk.whiteBright('Cores   '),
      ':',
      chalk.cyanBright(physicalCpuCount),
      chalk.cyan('physical'),
      '-',
      chalk.greenBright(cpus.length),
      chalk.green('logical')
    )
    console.log(chalk.whiteBright('Memory  '), ':', chalk.cyanBright((os.totalmem() / 1073741824).toFixed(2)), chalk.cyan('GB'))
    console.log(
      chalk.whiteBright('NodeJS  '),
      ':',
      chalk.cyanBright(process.version),
      '-',
      chalk.green('V8'),
      chalk.greenBright(`v${process.versions.v8}`)
    )
    console.log()
  }

  return systemInfo
}

module.exports = getSystemInfo()

if (require.main === module) {
  module.exports.print()
}
