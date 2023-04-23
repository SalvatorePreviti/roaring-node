#!/usr/bin/env node

const { execSync } = require("child_process");
const fs = require("fs");

const { OUTPUT_FILE_PATH, getBinaryOutputFilePath, runMain } = require("./lib/utils");
const { unity } = require("./lib/unity");

runMain(() => {
  const roaringNodeCpp = fs.readFileSync(OUTPUT_FILE_PATH, "utf8");

  const nodeVersion = parseInt(process.versions.node.split(".")[0], 10);
  if (nodeVersion >= 14) {
    execSync("npx lint-staged");
  }

  const unityResult = unity();
  console.log();
  if (unityResult.outputText !== roaringNodeCpp) {
    const chalk = require("chalk");
    console.error(
      chalk.redBright(
        `${chalk.underline.bold(
          "ERROR",
        )}: ${OUTPUT_FILE_PATH} is outdated or not a production version. Run \`npm run rebuild\` before committing and pushing.`,
      ),
    );
    process.exitCode = 1;
  } else {
    let moduleValid = false;
    // Try to load the module to check if it is valid
    try {
      require("../");
      moduleValid = true;
    } catch {}

    if (!moduleValid || !fs.existsSync(getBinaryOutputFilePath())) {
      execSync("npx node-pre-gyp rebuild", { stdio: "inherit" });
      // Try to load the module to check if it is valid
      require("../");
    }

    if (nodeVersion >= 12) {
      execSync("npm run lint", { stdio: "inherit" });
    }

    execSync("npm run test", { stdio: "inherit" });
  }
}, "prepush");
