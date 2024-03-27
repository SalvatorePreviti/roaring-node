#!/usr/bin/env node

async function test() {
  process.argv.push("--run");
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
