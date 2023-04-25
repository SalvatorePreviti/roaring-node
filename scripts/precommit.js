#!/usr/bin/env node

const { execSync } = require("child_process");
const { runMain } = require("./lib/utils");

runMain(() => {
  const nodeVersion = parseInt(process.versions.node.split(".")[0], 10);
  if (nodeVersion >= 14) {
    execSync("npx lint-staged", { stdio: "inherit" });
  }

  execSync("npx pretty-quick --staged", { stdio: "inherit" });
}, "precommit");
