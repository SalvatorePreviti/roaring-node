const bench = require('../scripts/benchmarks/bench')
const roaring = require('../')

const RoaringBitmap32 = roaring.RoaringBitmap32
const FastBitSet = require('fastbitset')

const N = 256 * 256

bench.suite('intersection (in place)', suite => {
  suite.detail(`${N} elements`)

  const b1 = new Uint32Array(N)
  const b2 = new Uint32Array(N)
  for (let i = 0; i < N; i++) {
    b1[i] = 3 * i + 5
    b2[i] = 6 * i + 5
  }

  suite.scope(() => {
    const s1 = new Set(b1)
    let s2
    suite.benchmark('Set', {
      setup() {
        s2 = new Set(b2)
      },
      fn() {
        for (const j of s1) {
          s2.delete(j)
        }
      }
    })
  })

  suite.scope(() => {
    const s1 = new FastBitSet(b1)
    let s2
    suite.benchmark('FastBitSet', {
      setup() {
        s2 = new FastBitSet(b2)
      },
      fn() {
        s2.intersection(s1)
      }
    })
  })

  suite.scope(() => {
    const s1 = new RoaringBitmap32(b1)
    let s2
    suite.benchmark('RoaringBitmap32', {
      setup() {
        s2 = new RoaringBitmap32(b2)
      },
      fn() {
        s2.andInPlace(s1)
      }
    })
  })
})

if (require.main === module) {
  bench.run()
}
