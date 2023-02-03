#!/usr/bin/env node --expose-gc

/* eslint-disable node/no-unsupported-features/node-builtins */
/* eslint-disable no-console */
/* global gc */

gc();
gc();

const { RoaringBitmap32, getRoaringUsedMemory } = require("../");

gc();
gc();

console.log();
console.log("RoaringUsedMemory", getRoaringUsedMemory());
console.log("RoaringBitmap32.getInstancesCount", RoaringBitmap32.getInstancesCount());

gc();
gc();

const a = new RoaringBitmap32([1, 2, 3]);
const b = new RoaringBitmap32([1, 2]);

for (let i = 0; i < 10000; i++) {
  RoaringBitmap32.andNot(a, b);
}

console.log();
console.log("RoaringUsedMemory", getRoaringUsedMemory());
console.log("RoaringBitmap32.getInstancesCount", RoaringBitmap32.getInstancesCount());
console.log();

gc();
gc();

console.table(process.memoryUsage());
console.time("allocation");

for (let i = 0; i < 10000000; i++) {
  RoaringBitmap32.andNot(a, b);
  if (i % 10000 === 0) {
    process.stdout.write(".");
    gc();
    gc();
  }
}
process.stdout.write("\n");
console.timeEnd("allocation");

console.log();
console.log("RoaringUsedMemory", getRoaringUsedMemory());
console.log("RoaringBitmap32.getInstancesCount", RoaringBitmap32.getInstancesCount());
console.log();

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
    console.log();
    console.log("RoaringUsedMemory", getRoaringUsedMemory());
    console.log("RoaringBitmap32.getInstancesCount", RoaringBitmap32.getInstancesCount());
    console.log();
  }, 150);
}, 150);
