#!/usr/bin/env node

const path = require("path");
const { runMain } = require("./lib/utils");

require("ts-node").register({ transpileOnly: process.argv.includes("--notypecheck") });

runMain(() => {
  const nodeVersion = parseInt(process.versions.node.split(".")[0], 10);
  if (nodeVersion < 14) {
    const url = require("url");
    if (!url.pathToFileURL) {
      url.pathToFileURL = (filePath) => {
        let pathName = path.resolve(filePath).replace(/\\/g, "/");
        if (!pathName.startsWith("/")) {
          pathName = `/${pathName}`;
        }
        return encodeURI(`file://${pathName}`).replace(/[?#]/g, encodeURIComponent);
      };
    }

    require("mocha/lib/nodejs/esm-utils.js").doImport = (v) => {
      if (typeof v === "object") {
        v = v.pathname;
      }
      try {
        v = url.fileURLToPath(v);
      } catch {}
      return new Promise((resolve) => resolve(require(v)));
    };
  }

  const { print: printSystemInfo } = require("./system-info.js");

  process.argv.push("test/**/*.test.ts");
  process.argv.push("test/*.test.ts");

  printSystemInfo();
  require("mocha/bin/mocha");
}, "test");
