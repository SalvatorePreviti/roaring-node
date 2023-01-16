const bench = require("../scripts/benchmarks/bench");
const roaring = require("../");

const RoaringBitmap32 = roaring.RoaringBitmap32;
const FastBitSet = require("fastbitset");

const N = 512 * 512;

bench.suite("intersection size", (suite) => {
  suite.detail(`${N} elements`);

  const b1 = new Uint32Array(N);
  const b2 = new Uint32Array(N);
  for (let i = 0; i < N; i++) {
    b1[i] = 3 * i + 5;
    b2[i] = 6 * i + 5;
  }

  suite.scope(() => {
    const s1 = new Set(b1);
    const s2 = new Set(b2);
    suite.benchmark("Set", () => {
      let answer = 0;
      if (s2.size > s1.size) {
        for (const j of s1) {
          if (s2.has(j)) {
            answer++;
          }
        }
      } else {
        for (const j of s2.values()) {
          if (s1.has(j)) {
            answer++;
          }
        }
      }
      return answer;
    });
  });

  suite.scope(() => {
    const s1 = new FastBitSet(b1);
    const s2 = new FastBitSet(b2);
    suite.benchmark("FastBitSet", () => {
      return s1.intersection_size(s2);
    });
  });

  suite.scope(() => {
    const s1 = new RoaringBitmap32(b1);
    const s2 = new RoaringBitmap32(b2);
    suite.benchmark("RoaringBitmap32", () => {
      return s1.andCardinality(s2);
    });
  });
});

if (require.main === module) {
  bench.run().catch((err) => {
    // eslint-disable-next-line no-console
    console.error(err);
  });
}
