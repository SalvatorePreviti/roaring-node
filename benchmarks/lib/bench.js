/* eslint no-console:0 */

const { Suite } = require('benchmark')

let currentSuite
let currentBenchmark
const suiteCompletedSymbol = Symbol()
const chalk = require('chalk')

module.exports = {
  suite(name, functor) {
    if (!functor.name) {
      Object.defineProperty(functor, 'name', { value: name })
    }
    currentSuite = new Suite(name)
    try {
      functor(currentSuite)
      return currentSuite
    } finally {
      currentSuite = undefined
    }
  },

  benchmark(name, functor) {
    currentBenchmark = { name }
    try {
      functor(currentBenchmark)
      if (!currentBenchmark.fn.name) {
        Object.defineProperty(currentBenchmark.fn, 'name', { value: `${currentSuite.name}:${name}` })
      }
      currentSuite.add(currentBenchmark)
    } finally {
      currentBenchmark = undefined
    }
    return module.exports
  },

  before(functor) {
    if (currentBenchmark) {
      currentBenchmark.onStart = functor
    } else if (currentSuite) {
      currentSuite.on('start', functor)
    }
    return module.exports
  },

  after(functor) {
    if (currentBenchmark) {
      currentBenchmark.onComplete = functor
    } else if (currentSuite) {
      currentSuite.on('complete', functor)
    }
    return module.exports
  },

  loop(functor) {
    currentBenchmark.fn = functor
    return module.exports
  },

  run(suite, options = { async: true }) {
    if (!suite[suiteCompletedSymbol]) {
      suite[suiteCompletedSymbol] = true

      if (process.send && process.env.NODE_BENCHMARK_FORKED === '1') {
        suite
          .on('cycle', event => {
            process.send({
              type: 'cycle',
              target: String(event.target)
            })
          })
          .on('complete', function() {
            process.send({
              type: 'complete',
              fastest: this.filter('fastest').map('name')
            })
          })
      } else {
        suite
          .on('cycle', event => {
            console.log('', String(event.target))
          })
          .on('complete', function() {
            console.log('', `complete - fastest is ${this.filter('fastest').map('name')}`)
          })
      }
    }

    console.log('Suite', suite.name)
    return suite.run(options)
  }
}
