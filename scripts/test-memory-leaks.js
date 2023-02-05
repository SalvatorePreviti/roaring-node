#!/usr/bin/env node --expose-gc

/* eslint-disable node/no-unsupported-features/node-builtins */
/* eslint-disable no-console */
/* global gc */

async function testMemoryLeaks() {
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

  let a = new RoaringBitmap32([1, 2, 3]);
  let b = new RoaringBitmap32([1, 2]);

  console.time("buildup");

  const operation = (i) => {
    switch (i % 4) {
      case 1:
        return RoaringBitmap32.and(a, b);
      case 2:
        return RoaringBitmap32.xor(a, b);
      case 3:
        return RoaringBitmap32.andNot(a, b);
      default:
        return RoaringBitmap32.or(a, b);
    }
  };

  let promises = [];
  for (let i = 0; i < 10000; i++) {
    const bmp = operation(i);
    if (i < 5) {
      promises.push(bmp.toUint32ArrayAsync());
    }
    promises.push(bmp.serializeAsync(i & 4 ? "croaring" : i & 8 ? "portable" : "unsafe_frozen_croaring"));
  }

  console.log();
  console.log("RoaringUsedMemory", getRoaringUsedMemory());
  console.log("RoaringBitmap32.getInstancesCount", RoaringBitmap32.getInstancesCount());
  console.log();

  await Promise.all(promises);
  promises = null;

  console.timeEnd("buildup");
  console.log();

  gc();
  gc();

  console.table(process.memoryUsage());
  process.stdout.write("\nallocating");
  console.time("allocation");

  gc();
  gc();

  promises = [];
  for (let i = 0; i < 10000000; i++) {
    const bmp = operation(i);
    if (i % 100000 === 0) {
      process.stdout.write(".");
      gc();
      gc();
      promises.push(bmp.serializeAsync("croaring").then(() => undefined));
    }
  }
  await Promise.all(promises);
  promises = null;
  process.stdout.write("\n");
  console.timeEnd("allocation");

  console.log();
  console.log("RoaringUsedMemory", getRoaringUsedMemory());
  console.log("RoaringBitmap32.getInstancesCount", RoaringBitmap32.getInstancesCount());
  console.log();

  a = null;
  b = null;

  const promise = new Promise((resolve, reject) => {
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

        setTimeout(() => {
          if (getRoaringUsedMemory() !== 0) {
            reject(new Error(`Memory leak detected. ${getRoaringUsedMemory()} bytes are still allocated.`));
          }

          if (RoaringBitmap32.getInstancesCount() !== 0) {
            reject(
              new Error(`Memory leak detected. ${RoaringBitmap32.getInstancesCount()} instances are still allocated.`),
            );
          }
          resolve();
        });
      }, 150);
    }, 150);
  });

  await promise;
}

console.log("\n--- testMemoryLeaks ---");
console.time("testMemoryLeaks");
testMemoryLeaks()
  .then(() => {
    console.timeEnd("testMemoryLeaks");
    console.log("\n✅ Ok. No memory leak detected.\n");
  })
  .catch((err) => {
    console.log();
    console.error(err);
    process.exitCode = 1;
  });
