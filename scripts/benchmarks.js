#!/usr/bin/env node

const printSystemInfo = require('./printSystemInfo')
const chalk = require('chalk').default
const path = require('path')
const fs = require('fs')

function listBenchamrkFiles() {
  return new Promise((resolve, reject) => {
    const folder = path.resolve(path.join(__dirname, '../benchmarks'))
    fs.readdir(folder, (err, files) => {
      if (err) {
        reject(err)
      } else {
        resolve(
          files.filter(file => file.endsWith('.benchmark.js')).map(file => {
            return { name: file.substr(0, file.length - 13), path: path.join(folder, file) }
          })
        )
      }
    })
  })
}

async function benchmarks() {
  const files = await listBenchamrkFiles()

  console.log(chalk.green('*'), chalk.greenBright('running'), chalk.cyanBright(files.length), chalk.greenBright('benchmarks...'), '\n')
  for (const file of files) {
    const benchmark = require(file.path)
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
