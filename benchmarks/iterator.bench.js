const bench = require('../scripts/benchmarks/bench')
const roaring = require('../')

const RoaringBitmap32 = roaring.RoaringBitmap32
const N = 65536

bench.suite('iterator', (suite) => {
  suite.detail(`${N} elements`)

  const data = new Uint32Array(N)
  for (let i = 0; i < N; i++) {
    data[i] = 3 * i + 5
  }

  suite.scope(() => {
    const x = new Set(data)
    suite.benchmark('Set.iterator', () => {
      let n = 0
      for (const j of x) {
        n += j
      }
      return n
    })
  })

  suite.scope(() => {
    const x = new Set(data)
    suite.benchmark('Set.forEach', () => {
      let n = 0
      x.forEach((v) => {
        n += v
      })
      return n
    })
  })

  suite.scope(() => {
    const x = new RoaringBitmap32(data)
    suite.benchmark('RoaringBitmap32.iterator', () => {
      let n = 0
      for (const j of x) {
        n += j
      }
      return n
    })
  })

  suite.scope(() => {
    const x = new RoaringBitmap32(data)
    suite.benchmark('RoaringBitmap32.forEach', () => {
      let n = 0
      x.forEach((v) => {
        n += v
      })
      return n
    })
  })
})

if (require.main === module) {
  bench.run()
}
