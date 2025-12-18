#!/usr/bin/env node

const { runMain, spawnAsync } = require("./lib/utils");

runMain(async () => {
  const { print: printSystemInfo } = require("./system-info.js");

  printSystemInfo();

  const cliArgs = process.argv.slice(2);

  await spawnAsync("npx", ["vitest", "run", ...cliArgs], {
    env: process.env,
  });
}, "test");
