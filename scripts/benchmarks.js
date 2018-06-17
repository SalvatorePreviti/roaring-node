#!/usr/bin/env node

const systemInfo = require('./systemInfo')
const promiseMap = require('./lib/promiseMap')
const chalk = require('chalk').default
const path = require('path')
const fs = require('fs')
const { fork } = require('child_process')

function listBenchmarksAsync() {
  return new Promise((resolve, reject) => {
    const benchmarkPostfix = '.xxx.js'
    const folder = path.resolve(path.join(__dirname, '../benchmarks'))
    fs.readdir(folder, (err, files) => {
      if (err) {
        reject(err)
      } else {
        resolve(
          files.filter(file => file.endsWith(benchmarkPostfix)).map(file => {
            return { name: file.substr(0, file.length - benchmarkPostfix.length), path: path.join(folder, file) }
          })
        )
      }
    })
  })
}

function runBenchmarkAsync(benchmark) {
  return new Promise((resolve, reject) => {
    const forked = fork(benchmark.path, { env: { NODE_BENCHMARK_FORKED: 1 } })
    forked.on('error', error => {
      reject(error)
    })
    forked.on('message', message => {
      console.log(message)
    })
    forked.on('exit', code => {
      if (code !== 0) {
        const error = new Error(`Benchmark "${benchmark.name}" failed.`)
        error.stack = error.message
        reject(error)
      } else {
        resolve()
      }
      forked.disconnect()
    })
  })
}

async function benchmarks() {
  const benchmarks = await listBenchmarksAsync()

  console.log(chalk.green('*'), chalk.greenBright('running'), chalk.cyanBright(benchmarks.length), chalk.greenBright('benchmarks...'), '\n')
  await promiseMap(benchmarks, runBenchmarkAsync, systemInfo.physicalCpuCount)
}

function run() {
  systemInfo.print()
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
      console.error(chalk.red('*'), chalk.redBright('FAIL'), chalk.red('-'), error, '\n')
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
