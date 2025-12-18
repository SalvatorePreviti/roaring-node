import FastBitSet from "fastbitset";
import { bench, describe } from "vitest";
import roaringModule from "../index.js";
import { consume } from "./utils";

const { RoaringBitmap32 } = roaringModule;
const N = 1024 * 1024;

const setA = new Set<number>();
const setB = new Set<number>();
const fastA = new FastBitSet();
const fastB = new FastBitSet();
const roaringA = new RoaringBitmap32();
const roaringB = new RoaringBitmap32();

for (let i = 0; i < N; i++) {
  const valueA = 3 * i + 5;
  const valueB = 6 * i + 5;
  setA.add(valueA);
  setB.add(valueB);
  fastA.add(valueA);
  fastB.add(valueB);
  roaringA.add(valueA);
  roaringB.add(valueB);
}

describe("intersection (new)", () => {
  bench("Set", () => {
    const answer = new Set<number>();
    if (setB.size > setA.size) {
      for (const value of setA) {
        if (setB.has(value)) {
          answer.add(value);
        }
      }
    } else {
      for (const value of setB) {
        if (setA.has(value)) {
          answer.add(value);
        }
      }
    }
    consume(answer);
  });

  bench("FastBitSet", () => {
    consume(fastA.new_intersection(fastB));
  });

  bench("RoaringBitmap32", () => {
    consume(RoaringBitmap32.and(roaringA, roaringB));
  });
});
