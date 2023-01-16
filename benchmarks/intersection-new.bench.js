const bench = require("../scripts/benchmarks/bench");
const roaring = require("../");

const RoaringBitmap32 = roaring.RoaringBitmap32;
const FastBitSet = require("fastbitset");

const N = 1024 * 1024;

bench.suite("intersection (new)", (suite) => {
  suite.detail(`${N} elements`);

  suite.scope(() => {
    const s1 = new Set();
    const s2 = new Set();
    for (let i = 0; i < N; i++) {
      s1.add(3 * i + 5);
      s2.add(6 * i + 5);
    }
    suite.benchmark("Set", () => {
      const answer = new Set();
      if (s2.size > s1.size) {
        for (const j of s1) {
          if (s2.has(j)) {
            answer.add(j);
          }
        }
      } else {
        for (const j of s2) {
          if (s1.has(j)) {
            answer.add(j);
          }
        }
      }
      return answer;
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
      return s1.new_intersection(s2);
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
      return RoaringBitmap32.and(s1, s2);
    });
  });
});

if (require.main === module) {
  bench.run().catch((err) => {
    // eslint-disable-next-line no-console
    console.error(err);
  });
}
