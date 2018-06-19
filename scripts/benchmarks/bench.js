const Benchmark = require('benchmark')
const benchReport = require('./benchReport')

const suitesDeclarations = []

function errorToString(error) {
  return error.stack || String(error || 'Error')
}

const actions =
  process.send && process.env.NODE_BENCHMARK_FORKED === '1'
    ? {
        suiteStart() {},
        suiteReport(report) {
          process.send(report)
        }
      }
    : {
        suiteStart: benchReport.printSuiteName,
        suiteReport: benchReport.printSuiteReport
      }

function pushBenchResult(results, b) {
  const failed = b.error || b.aborted
  results.push({
    name: b.name,
    aborted: b.aborted,
    error: b.error !== undefined && errorToString(b.error),
    hz: failed ? 0 : b.hz || 0,
    ops: failed ? '' : b.hz.toFixed(2).replace(/\B(?=(\d{3})+\b)/g, ','),
    rme: failed ? '' : b.stats.rme.toFixed(2),
    runs: failed ? '' : `${b.stats.sample.length}`,
    diff: '',
    fastest: false
  })
}

class BenchSuite {
  constructor(name) {
    this.name = name
    this.results = []
    this.details = []
  }

  detail(text) {
    this.details.push(text)
  }

  benchmark(name, fn) {
    let b

    if (typeof fn === 'function') {
      Object.defineProperty(fn, 'name', { value: `${this.name}:${fn.name || name}`, configurable: true, writable: true })
      b = new Benchmark({ name, fn, async: false })
    } else {
      b = new Benchmark({ name, fn: fn.fn, async: false })
    }

    if (typeof fn.setup === 'function') {
      b.on('start cycle', () => {
        fn.setup(this)
      })
    }

    b.on('complete', () => {
      pushBenchResult(this.results, b)
    })

    b.run()
    return this
  }
}

async function runSuite(suiteDeclaration) {
  const benchSuite = new BenchSuite(suiteDeclaration.name)
  let fastest = null
  let error = null
  try {
    actions.suiteStart(suiteDeclaration.name)

    await suiteDeclaration.fn(benchSuite)

    let fastValue = 0
    for (const b of benchSuite.results) {
      if (fastValue < b.hz) {
        fastValue = b.hz
        fastest = b
      }
    }

    if (fastest) {
      fastest.fastest = true
      for (const b of benchSuite.results) {
        b.diff = b.fastest ? '' : `-${((1.0 - b.hz / fastest.hz) * 100).toFixed(2)}%`
      }
    }
  } catch (e) {
    error = errorToString(e)
  } finally {
    actions.suiteReport({
      type: 'suiteReport',
      name: suiteDeclaration.name,
      details: benchSuite.details,
      error,
      benchs: benchSuite.results,
      fastest: fastest && fastest.name
    })
  }
}

module.exports = {
  /**
   * Defines a new benchmark suite
   *
   * @param {string} name The name of the suite. Required.
   * @param {(BenchSuite)} fn The content of the suite. Required
   * @returns {void}
   */
  suite(name, fn) {
    if (!fn.name) {
      Object.defineProperty(fn, 'name', { value: name, configurable: true, writable: true })
    }
    suitesDeclarations.push({ name, fn })
  },

  async run() {
    for (const suite of suitesDeclarations.slice().sort(x => x.name)) {
      await runSuite(suite)
    }
  }
}
