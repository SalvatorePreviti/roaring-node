#!/usr/bin/env node

async function test() {
  process.argv.push("--run");
  process.argv.push("--reporter", "verbose");
  process.argv.push("test/aligned-buffers/bufferAlignedAllocShared.test.ts");
  return import("vitest/vitest.mjs");
}

test()
  .then(() => {
    console.log("Test OK");
  })
  .catch((err) => {
    console.error(err);
    if (!process.exitCode) {
      process.exitCode = 1;
    }
  });
