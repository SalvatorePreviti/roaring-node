const bench = require("../scripts/benchmarks/bench");
const roaring = require("../");

const RoaringBitmap32 = roaring.RoaringBitmap32;
const FastBitSet = require("fastbitset");

const N = 1024 * 1024;

bench.suite("union (new) ", (suite) => {
  suite.detail(`${N} elements`);

  suite.scope(() => {
    const s1 = new Set();
    const s2 = new Set();
    for (let i = 0; i < N; i++) {
      s1.add(3 * i + 5);
      s2.add(6 * i + 5);
    }
    suite.benchmark("Set", () => {
      genericSetUnion(s1, s2);
    });
  });

  suite.scope(() => {
    const s1 = new FastBitSet();
    const s2 = new FastBitSet();
    for (let i = 0; i < N; i++) {
      s1.add(3 * i + 5);
      s2.add(6 * i + 5);
    }
    suite.benchmark("FastBitSet", () => {
      s1.new_union(s2);
    });
  });

  suite.scope(() => {
    const s1 = new RoaringBitmap32();
    const s2 = new RoaringBitmap32();
    for (let i = 0; i < N; i++) {
      s1.add(3 * i + 5);
      s2.add(6 * i + 5);
    }
    suite.benchmark("RoaringBitmap32", () => {
      RoaringBitmap32.or(s1, s2);
    });
  });
});

if (require.main === module) {
  bench.run().catch((err) => {
    // eslint-disable-next-line no-console
    console.error(err);
  });
}

function genericSetUnion(set1, set2) {
  const answer = new Set(set1);
  for (const j of set2) {
    answer.add(j);
  }
  return answer;
}
