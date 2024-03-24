#!/usr/bin/env node

async function test() {
  process.argv.push("--run");
  process.argv.push("--no-file-parallelism");
  process.argv.push("--reporter", "verbose");
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
