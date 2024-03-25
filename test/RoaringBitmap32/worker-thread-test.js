// eslint-disable-next-line node/no-unsupported-features/node-builtins
const { parentPort } = require("worker_threads");

process.on("uncaughtException", (err) => {
  // eslint-disable-next-line no-console
  console.error("worker thread uncaughtException", err);
  if (parentPort) {
    parentPort.postMessage(err);
  }
});

function expectArrayEqual(a, b) {
  if (a.length !== b.length) {
    throw new Error("array length not equal");
  }
  for (let i = 0; i < a.length; i++) {
    if (a[i] !== b[i]) {
      throw new Error(`array not equal at index ${i}`);
    }
  }
}

async function main() {
  const { RoaringBitmap32 } = require("../..");

  const bitmap = new RoaringBitmap32();
  bitmap.add(1);
  bitmap.add(2);
  bitmap.add(100);
  bitmap.add(101);

  bitmap.addRange(105, 109);
  bitmap.addMany([0x7fffffff, 0xfffffffe, 0xffffffff]);

  expectArrayEqual(bitmap.toArray(), [1, 2, 100, 101, 105, 106, 107, 108, 0x7fffffff, 0xfffffffe, 0xffffffff]);

  const serialized = await bitmap.serializeAsync("portable");

  const bitmap2 = await RoaringBitmap32.deserializeAsync(serialized, "portable");
  expectArrayEqual(bitmap2.toArray(), [1, 2, 100, 101, 105, 106, 107, 108, 0x7fffffff, 0xfffffffe, 0xffffffff]);

  bitmap2.add(121);

  const bitmap3 = RoaringBitmap32.orMany([bitmap, bitmap2]);

  expectArrayEqual(Array.from(bitmap3), [1, 2, 100, 101, 105, 106, 107, 108, 121, 0x7fffffff, 0xfffffffe, 0xffffffff]);
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
