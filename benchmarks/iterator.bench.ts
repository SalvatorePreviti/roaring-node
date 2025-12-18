import { bench, describe } from "vitest";
import roaringModule from "../index.js";
import { consume } from "./utils.js";

const { RoaringBitmap32 } = roaringModule;
const N = 65536;

const data = new Uint32Array(N);
for (let i = 0; i < N; i++) {
  data[i] = 3 * i + 5;
}

const setData = new Set(data);
const roaringData = new RoaringBitmap32(data);

describe("iterator", () => {
  bench("Set.iterator", () => {
    let total = 0;
    for (const value of setData) {
      total ^= value;
    }
    consume(total);
  });

  bench("Set.forEach", () => {
    let total = 0;
    setData.forEach((value) => {
      total ^= value;
    });
    consume(total);
  });

  bench("RoaringBitmap32.iterator", () => {
    let total = 0;
    for (const value of roaringData) {
      total ^= value;
    }
    consume(total);
  });

  bench("RoaringBitmap32.forEach", () => {
    let total = 0;
    roaringData.forEach((value) => {
      total ^= value;
    });
    consume(total);
  });
});
