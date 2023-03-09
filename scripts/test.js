#!/usr/bin/env node

console.time("test");
process.on("exit", () => {
  console.timeEnd("test");
});

require("ts-node/register");

const { print: printSystemInfo } = require("./systemInfo.js");

require("mocha/lib/nodejs/esm-utils.js").doImport = (v) => {
  if (typeof v === "object") {
    v = v.pathname;
  }
  return new Promise((resolve) => resolve(require(v)));
};

printSystemInfo();

process.argv.push("test/**/*.test.ts");
process.argv.push("test/*.test.ts");
require("mocha/bin/mocha");
