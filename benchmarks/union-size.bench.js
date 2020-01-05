const bench = require('../scripts/benchmarks/bench')
const roaring = require('../')

const RoaringBitmap32 = roaring.RoaringBitmap32
const FastBitSet = require('fastbitset')

const N = 512 * 512

bench.suite('union size', suite => {
  suite.detail(`${N} elements`)

  const b1 = new Uint32Array(N)
  const b2 = new Uint32Array(N)
  for (let i = 0; i < N; i++) {
    b1[i] = 3 * i + 5
    b2[i] = 6 * i + 5
  }

  suite.scope(() => {
    const s1 = new Set(b1)
    const s2 = new Set(b2)
    suite.benchmark('Set', () => {
      let answer
      if (s1.size > s2.size) {
        answer = s1.size
        for (const j of s2) {
          if (!s1.has(j) && s2.has(j)) {
            answer++
          }
        }
      } else {
        answer = s2.size
        for (const j of s1) {
          if (!s2.has(j) && s1.has(j)) {
            answer++
          }
        }
      }
      return answer
    })
  })

  suite.scope(() => {
    const s1 = new FastBitSet(b1)
    const s2 = new FastBitSet(b2)
    suite.benchmark('FastBitSet', () => {
      s1.union_size(s2)
    })
  })

  suite.scope(() => {
    const s1 = new RoaringBitmap32(b1)
    const s2 = new RoaringBitmap32(b2)
    suite.benchmark('RoaringBitmap32', () => {
      s1.orCardinality(s2)
    })
  })
})

if (require.main === module) {
  bench.run()
}
