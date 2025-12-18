#!/usr/bin/env node

const { runMain } = require("./lib/utils");

require("tsx/cjs");

runMain(() => {
  const url = require("node:url");
  require("mocha/lib/nodejs/esm-utils.js").doImport = (v) => {
    if (typeof v === "object") {
      v = url.fileURLToPath(v);
    }
    return new Promise((resolve) => resolve(require(v)));
  };

  const { print: printSystemInfo } = require("./system-info.js");

  process.argv.push("test/**/*.test.ts");
  process.argv.push("test/*.test.ts");

  printSystemInfo();

  require("mocha/bin/mocha");
}, "test");
