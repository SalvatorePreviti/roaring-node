#!/usr/bin/env node

const chalk = require('chalk').default
const os = require('os')
const instructionSet = require('../lib/instructionSet')

/**
 * Prints system info to the standard console
 * @returns {void}
 */
function printSystemInfo() {
  console.log()
  console.log(
    chalk.whiteBright('Platform '),
    ':',
    chalk.cyanBright(os.type()),
    chalk.greenBright(os.release()),
    chalk.yellowBright(process.arch)
  )
  console.log(chalk.whiteBright('Memory   '), ':', chalk.cyanBright((os.totalmem() / 1073741824).toFixed(2)), chalk.cyan('GB'))
  console.log(
    chalk.whiteBright('Processor'),
    ':',
    chalk.cyanBright(os.cpus()[0].model),
    instructionSet !== 'PLAIN' ? chalk.greenBright(instructionSet) : ''
  )
  console.log(
    chalk.whiteBright('NodeJS   '),
    ':',
    chalk.cyanBright(process.version),
    '-',
    chalk.green('V8'),
    chalk.greenBright(`v${process.versions.v8}`)
  )
  console.log()
}

module.exports = printSystemInfo

if (require.main === module) {
  printSystemInfo()
}
