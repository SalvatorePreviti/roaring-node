#!/usr/bin/env node

const printSystemInfo = require('./printSystemInfo')
const globby = require('globby')
const chalk = require('chalk').default
const path = require('path')

async function benchmarks() {
  const files = await globby(path.join(__dirname, '../benchmarks/**/*.benchmark.js'))

  console.log(chalk.green('*'), chalk.greenBright('running'), chalk.cyanBright(files.length), chalk.greenBright('benchmarks...'), '\n')

  for (const file of files) {
    const benchmark = require(file)
    await benchmark()
    console.log()
  }
}

function run() {
  printSystemInfo()
  const benchmarksCompletedMessage = `\n${chalk.green('*')} ${chalk.greenBright('benchmarks completed')}`
  console.time(benchmarksCompletedMessage)
  return benchmarks()
    .then(() => {
      console.timeEnd(benchmarksCompletedMessage)
      console.log()
      return true
    })
    .catch(error => {
      console.log()
      console.error(chalk.red('*'), chalk.redBright('benchmarks failed'), chalk.red('-'), error, '\n')
      if (!process.exitCode) {
        process.exitCode = 1
      }
      return false
    })
}

benchmarks.run = run

module.exports = benchmarks

if (require.main === module) {
  run()
}
