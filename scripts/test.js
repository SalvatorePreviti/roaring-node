#!/usr/bin/env node

const { spawnAsync } = require("./lib/utils");

async function test() {
  await spawnAsync("npx", ["vitest", "--run"]);
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
