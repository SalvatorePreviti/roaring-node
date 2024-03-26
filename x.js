function xxx() {
  const { RoaringBitmap32 } = require("./");

  const bitmap = new RoaringBitmap32();
  bitmap.add(1);
  bitmap.add(2);
  bitmap.add(100);
  bitmap.add(101);

  bitmap.addRange(105, 109);
  bitmap.addMany([0x7fffffff, 0xfffffffe, 0xffffffff]);

  const expectArrayEqual = (a, b) => {
    if (a.length !== b.length) {
      throw new Error("array length not equal");
    }
    for (let i = 0; i < a.length; i++) {
      if (a[i] !== b[i]) {
        throw new Error(`array not equal at index ${i}`);
      }
    }
  };

  expectArrayEqual(bitmap.toArray(), [1, 2, 100, 101, 105, 106, 107, 108, 0x7fffffff, 0xfffffffe, 0xffffffff]);

  const serialized = bitmap.serialize("portable");

  const bitmap2 = RoaringBitmap32.deserialize(serialized, "portable");
  expectArrayEqual(bitmap2.toArray(), [1, 2, 100, 101, 105, 106, 107, 108, 0x7fffffff, 0xfffffffe, 0xffffffff]);

  bitmap2.add(121);

  const bitmap3 = RoaringBitmap32.orMany([bitmap, bitmap2]);

  expectArrayEqual(Array.from(bitmap3), [1, 2, 100, 101, 105, 106, 107, 108, 121, 0x7fffffff, 0xfffffffe, 0xffffffff]);
}

xxx();

// delete node require.cache
const exps = require("module")._cache[require.resolve("./")].exports;
for (const k in exps) {
  delete exps[k];
}
console.log(exps);

setInterval(() => gc(), 1).unref();
setTimeout(() => {
  console.log("ok");
}, 2000);
