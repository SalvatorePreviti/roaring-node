import FastBitSet from "fastbitset";
import { bench, describe } from "vitest";
import roaringModule from "../../index.js";

const { RoaringBitmap32 } = roaringModule;
const N = 256 * 256;

const left = new Uint32Array(N);
const right = new Uint32Array(N);
for (let i = 0; i < N; i++) {
  left[i] = 3 * i + 5;
  right[i] = 6 * i + 5;
}

const setLeft = new Set(left);
const fastLeft = new FastBitSet(left);
const roaringLeft = new RoaringBitmap32(left);

describe("intersection (in place)", () => {
  bench("Set", () => {
    const target = new Set(right);
    for (const value of setLeft) {
      target.delete(value);
    }
  });

  bench("FastBitSet", () => {
    const target = new FastBitSet(right);
    target.intersection(fastLeft);
  });

  bench("RoaringBitmap32", () => {
    const target = new RoaringBitmap32(right);
    target.andInPlace(roaringLeft);
  });
});
