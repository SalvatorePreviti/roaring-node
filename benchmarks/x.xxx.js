const bench = require('./lib/bench')

module.exports = bench.suite('hello world', () => {
  bench.benchmark('t1', () => {
    bench.loop(() => {})
  })
})

if (require.main === module) {
  bench.run(module.exports)
}

/*
const benchmark = require('benchmark')

const suite = new benchmark.Suite('suite name')

suite.add({
  name: 'FastBitSet (creates new bitset)',
  onStart() {
    console.log('cycle!')
    throw new Error('xxx')
  },
  onComplete() {},
  fn() {}
})

suite.run({ async: true })

module.exports = suite*/
