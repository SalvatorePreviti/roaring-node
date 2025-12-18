import FastBitSet from "fastbitset";
import { bench, describe } from "vitest";
import roaringModule from "../index.js";
import { consume } from "./utils";

const { RoaringBitmap32 } = roaringModule;
const N = 512 * 512;

const left = new Uint32Array(N);
const right = new Uint32Array(N);
for (let i = 0; i < N; i++) {
  left[i] = 3 * i + 5;
  right[i] = 6 * i + 5;
}

const setLeft = new Set(left);
const setRight = new Set(right);
const fastLeft = new FastBitSet(left);
const fastRight = new FastBitSet(right);
const roaringLeft = new RoaringBitmap32(left);
const roaringRight = new RoaringBitmap32(right);

describe("intersection size", () => {
  bench("Set", () => {
    let answer = 0;
    if (setRight.size > setLeft.size) {
      for (const value of setLeft) {
        if (setRight.has(value)) {
          answer++;
        }
      }
    } else {
      for (const value of setRight) {
        if (setLeft.has(value)) {
          answer++;
        }
      }
    }
    consume(answer);
  });

  bench("FastBitSet", () => {
    consume(fastLeft.intersection_size(fastRight));
  });

  bench("RoaringBitmap32", () => {
    consume(roaringLeft.andCardinality(roaringRight));
  });
});
