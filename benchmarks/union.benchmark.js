/* eslint no-console: 0 */

const roaring = require('../')
const RoaringBitmap32 = roaring.RoaringBitmap32

const FastBitSet = require('fastbitset')
const benchmark = require('benchmark')

function genericSetIntersection(set1, set2) {
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

function genericInplaceSetIntersection(set1, set2) {
  for (const j of set2) {
    if (!set1.has(j)) {
      set1.delete(j)
    }
  }
  return set1
}

function genericSetIntersectionCard(set1, set2) {
  let answer = 0
  if (set2.length > set1.length) {
    for (const j of set1) {
      if (set2.has(j)) {
        answer++
      }
    }
  } else {
    for (const j of set2.values()) {
      if (set1.has(j)) {
        answer++
      }
    }
  }
  return answer
}

function genericSetUnion(set1, set2) {
  const answer = new Set(set1)
  for (const j of set2) {
    answer.add(j)
  }
  return answer
}

function genericInplaceSetUnion(set1, set2) {
  for (const j of set2) {
    set1.add(j)
  }
  return set1
}

function genericSetUnionCard(set1, set2) {
  return set1.size + set2.size - genericSetIntersectionCard(set1, set2)
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
  return set1.size - genericSetIntersectionCard(set1, set2)
}

const N = 1024 * 1024

function runBenchmarks() {
  console.log('starting union query benchmark')
  const b1 = new FastBitSet()
  const r1 = new RoaringBitmap32()
  const r2 = new RoaringBitmap32()
  const b2 = new FastBitSet()
  const s1 = new Set()
  const s2 = new Set()
  for (let i = 0; i < N; i++) {
    b1.add(3 * i + 5)
    s1.add(3 * i + 5)
    r1.add(3 * i + 5)
    r2.add(6 * i + 5)
    b2.add(6 * i + 5)
    s2.add(6 * i + 5)
  }
  const suite = new benchmark.Suite()
  suite
    .add('Set', () => {
      genericSetUnion(s1, s2)
    })
    .add('FastBitSet (creates new bitset)', () => {
      b1.new_union(b2)
    })
    .add('RoaringBitmap32', () => {
      RoaringBitmap32.or(r1, r2)
    })

  suite
    .on('cycle', event => {
      console.log(String(event.target))
    })
    .on('complete', function() {
      console.log(`Fastest is ${this.filter('fastest').map('name')}`)
    })
    .run({ async: false })
}

module.exports = runBenchmarks

if (require.main === module) {
  runBenchmarks()
}
