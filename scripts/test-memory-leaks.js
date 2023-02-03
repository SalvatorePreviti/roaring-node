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

  let promises = [];
  for (let i = 0; i < 10000; i++) {
    const bmp = RoaringBitmap32.andNot(a, b);
    promises.push(bmp.serializeAsync(i & 1 ? "croaring" : i & 2 ? "portable" : "unsafe_frozen_croaring"));
  }

  console.log();
  console.log("RoaringUsedMemory", getRoaringUsedMemory());
  console.log("RoaringBitmap32.getInstancesCount", RoaringBitmap32.getInstancesCount());
  console.log();

  await Promise.all(promises);
  promises = null;

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

  a = null;
  b = null;

  const promise = new Promise((resolve) => {
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

        resolve();
      }, 150);
    }, 150);
  });

  await promise;
}

testMemoryLeaks()
  .then(() => {
    console.log("Done");
  })
  .catch((err) => {
    console.log();
    console.error(err);
    process.exitCode = 1;
  });
