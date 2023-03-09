#!/usr/bin/env node

const { execSync } = require("child_process");
const fs = require("fs");

const { OUTPUT_FILE_PATH, BINARY_OUTPUT_FILE_PATH, unity } = require("./lib/unity");

// Verifies that is the production version

const roaringNodeCpp = fs.readFileSync(OUTPUT_FILE_PATH, "utf8");

const unityResult = unity();
console.log();
if (unityResult.outputText !== roaringNodeCpp) {
  throw new Error(
    "roaring-node.cpp is not the production version. Run `npm run rebuild` before committing and pushing.",
  );
}

if (!fs.existsSync(BINARY_OUTPUT_FILE_PATH)) {
  process.argv.push("build");
  require("node-gyp/bin/node-gyp.js");
}

execSync("npm run lint", { stdio: "inherit" });

execSync("npm run test", { stdio: "inherit" });
