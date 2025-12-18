#!/usr/bin/env node

const { runMain, spawnAsync } = require("./lib/utils");

runMain(async () => {
  const cliArgs = process.argv.slice(2);

  await spawnAsync("npx", ["vitest", "run", ...cliArgs], {
    env: process.env,
  });
}, "test");
