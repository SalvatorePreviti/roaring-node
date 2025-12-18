import { bench, describe } from "vitest";
import roaringModule from "../../index.js";

const { RoaringBitmap32 } = roaringModule;
const N = 65535;

const data = new Uint32Array(N);
for (let i = 0; i < N; i++) {
  data[i] = 3 * i + 5;
}
const dataArray = Array.from(data);

describe("add", () => {
  bench("Set.add", () => {
    const x = new Set<number>();
    for (let i = 0; i < N; ++i) {
      x.add(data[i]);
    }
  });

  bench("RoaringBitmap32.tryAdd", () => {
    const x = new RoaringBitmap32();
    for (let i = 0; i < N; ++i) {
      x.tryAdd(data[i]);
    }
  });

  bench("RoaringBitmap32.add", () => {
    const x = new RoaringBitmap32();
    for (let i = 0; i < N; ++i) {
      x.add(data[i]);
    }
  });

  bench("RoaringBitmap32.addMany Array", () => {
    const x = new RoaringBitmap32();
    x.addMany(dataArray);
  });

  bench("RoaringBitmap32.addMany Uint32Array", () => {
    const x = new RoaringBitmap32();
    x.addMany(data);
  });
});
