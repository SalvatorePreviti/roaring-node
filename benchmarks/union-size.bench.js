const bench = require('../scripts/benchmarks/bench')
const roaring = require('../')
const RoaringBitmap32 = roaring.RoaringBitmap32
const FastBitSet = require('fastbitset')
const N = 512 * 512

bench.suite('union size', suite => {
  suite.detail(`${N} elements`)

  const b1 = new Uint32Array(N * 2)
  const b2 = new Uint32Array(N * 2)
  for (let i = 0; i < N; i++) {
    b1[i] = 3 * i + 5
    b2[i] = 6 * i + 5
  }

  let s1, s2

  s1 = new Set(b1)
  s2 = new Set(b2)
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

  s1 = new FastBitSet(b1)
  s2 = new FastBitSet(b2)
  suite.benchmark('FastBitSet', () => {
    s1.union_size(s2)
  })

  s1 = new RoaringBitmap32(b1)
  s2 = new RoaringBitmap32(b2)
  suite.benchmark('RoaringBitmap32', () => {
    s1.orCardinality(s2)
  })
})

if (require.main === module) {
  bench.run()
}

/*


function genericSetunion(set1, set2) {
  const answer = new Set()
  if (set2.length > set1.length) {
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

function genericInplaceSetunion(set1, set2) {
  for (const j of set2) {
    if (!set1.has(j)) {
      set1.delete(j)
    }
  }
  return set1
}

function genericInplaceSetUnion(set1, set2) {
  for (const j of set2) {
    set1.add(j)
  }
  return set1
}

function genericSetUnionCard(set1, set2) {
  return set1.size + set2.size - genericSetunionCard(set1, set2)
}

function genericSetDifference(set1, set2) {
  const answer = new Set(set1)
  for (const j of set2) {
    answer.delete(j)
  }
  return answer
}

function genericInplaceSetDifference(set1, set2) {
  for (const j of set2) {
    set1.delete(j)
  }
  return set1
}

function genericSetDifferenceCard(set1, set2) {
  return set1.size - genericSetunionCard(set1, set2)
}
*/
