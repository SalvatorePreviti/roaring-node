const bench = require('../scripts/benchmarks/bench')
const roaring = require('../')
const RoaringBitmap32 = roaring.RoaringBitmap32
const N = 65535

bench.suite('add', suite => {
  suite.detail(`${N} elements`)

  const data = new Uint32Array(N)
  for (let i = 0; i < N; i++) {
    data[i] = 3 * i + 5
  }

  const dataArray = Array.from(data)

  suite.scope(() => {
    let x
    suite.benchmark('Set.add', {
      setup() {
        x = new Set()
      },
      fn() {
        for (let i = 0; i < N; ++i) {
          x.add(data[i])
        }
      }
    })
  })

  suite.scope(() => {
    let x
    suite.benchmark('RoaringBitmap32.tryAdd', {
      setup() {
        x = new RoaringBitmap32()
      },
      fn() {
        for (let i = 0; i < N; ++i) {
          x.tryAdd(data[i])
        }
      }
    })
  })

  suite.scope(() => {
    let x
    suite.benchmark('RoaringBitmap32.add', {
      setup() {
        x = new RoaringBitmap32()
      },
      fn() {
        for (let i = 0; i < N; ++i) {
          x.add(data[i])
        }
      }
    })
  })

  suite.scope(() => {
    let x
    suite.benchmark('RoaringBitmap32.addMany Array', {
      setup() {
        x = new RoaringBitmap32()
      },
      fn() {
        x.addMany(dataArray)
      }
    })
  })

  suite.scope(() => {
    let x
    suite.benchmark('RoaringBitmap32.addMany Uint32Array', {
      setup() {
        x = new RoaringBitmap32()
      },
      fn() {
        x.addMany(data)
      }
    })
  })
})

if (require.main === module) {
  bench.run()
}
