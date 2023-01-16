#!/usr/bin/env node --expose-gc

/* eslint-disable node/no-unsupported-features/node-builtins */
/* eslint-disable no-console */
/* global gc */

gc();
gc();

const RoaringBitmap = require("../RoaringBitmap32");

gc();
gc();

const a = new RoaringBitmap([1, 2, 3]);
const b = new RoaringBitmap([1, 2]);

for (let i = 0; i < 10000; i++) {
  RoaringBitmap.andNot(a, b);
}
gc();
gc();

console.table(process.memoryUsage());
console.time("allocation");

for (let i = 0; i < 10000000; i++) {
  RoaringBitmap.andNot(a, b);
  if (i % 10000 === 0) {
    process.stdout.write(".");
    gc();
    gc();
  }
}
process.stdout.write("\n");
console.timeEnd("allocation");

for (let i = 0; i < 10; ++i) {
  gc();
}
setTimeout(() => {
  for (let i = 0; i < 10; ++i) {
    gc();
  }
  setTimeout(() => {
    for (let i = 0; i < 10; ++i) {
      gc();
    }
    console.table(process.memoryUsage());
  }, 150);
}, 150);
