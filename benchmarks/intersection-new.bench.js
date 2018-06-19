const bench = require('../scripts/benchmarks/bench')
const roaring = require('../')
const RoaringBitmap32 = roaring.RoaringBitmap32
const FastBitSet = require('fastbitset')
const N = 1024 * 1024

bench.suite('intersection (new)', suite => {
  suite.detail(`${N} elements`)

  let s1, s2

  s1 = new Set()
  s2 = new Set()
  for (let i = 0; i < N; i++) {
    s1.add(3 * i + 5)
    s2.add(6 * i + 5)
  }
  suite.benchmark('Set', () => {
    genericSetIntersection(s1, s2)
  })

  s1 = new FastBitSet()
  s2 = new FastBitSet()
  for (let i = 0; i < N; i++) {
    s1.add(3 * i + 5)
    s2.add(6 * i + 5)
  }
  suite.benchmark('FastBitSet', () => {
    s1.new_intersection(s2)
  })

  s1 = new RoaringBitmap32()
  s2 = new RoaringBitmap32()
  for (let i = 0; i < N; i++) {
    s1.add(3 * i + 5)
    s2.add(6 * i + 5)
  }
  suite.benchmark('RoaringBitmap32', () => {
    RoaringBitmap32.and(s1, s2)
  })
})

function genericSetIntersection(set1, set2) {
  const answer = new Set()
  if (set2.size > set1.size) {
    for (const j of set1) {
      if (set2.has(j)) {
        answer.add(j)
      }
    }
  } else {
    for (const j of set2) {
      if (set1.has(j)) {
        answer.add(j)
      }
    }
  }
  return answer
}

if (require.main === module) {
  bench.run()
}
