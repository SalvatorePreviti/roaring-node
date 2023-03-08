import { expect } from "chai";

// eslint-disable-next-line node/no-unsupported-features/node-builtins
import { parentPort } from "worker_threads";

async function main() {
  const RoaringBitmap32 = (await import("../../../")).RoaringBitmap32;

  const bitmap = new RoaringBitmap32();
  bitmap.add(1);
  bitmap.add(2);
  bitmap.add(100);
  bitmap.add(101);

  bitmap.addRange(105, 109);
  bitmap.addMany([0x7fffffff, 0xfffffffe, 0xffffffff]);

  expect(bitmap.toArray()).deep.equal([1, 2, 100, 101, 105, 106, 107, 108, 0x7fffffff, 0xfffffffe, 0xffffffff]);

  const serialized = await bitmap.serializeAsync("portable");

  const bitmap2 = await RoaringBitmap32.deserializeAsync(serialized, "portable");
  expect(bitmap2.toArray()).deep.equal([1, 2, 100, 101, 105, 106, 107, 108, 0x7fffffff, 0xfffffffe, 0xffffffff]);

  bitmap2.add(121);

  const bitmap3 = RoaringBitmap32.orMany([bitmap, bitmap2]);

  expect(bitmap3.toArray()).deep.equal([1, 2, 100, 101, 105, 106, 107, 108, 121, 0x7fffffff, 0xfffffffe, 0xffffffff]);
}

main()
  .then(() => {
    if (parentPort) {
      parentPort.postMessage("ok");
    } else {
      // eslint-disable-next-line no-console
      console.log("ok");
    }
  })
  .catch((e) => {
    // eslint-disable-next-line no-console
    console.error("worker thread error", e);
    if (parentPort) {
      parentPort.postMessage(e);
    }
  });
