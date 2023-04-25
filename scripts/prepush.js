#!/usr/bin/env node

const { execSync } = require("child_process");
const fs = require("fs");

const { CPP_UNITY_FILE_PATH, runMain } = require("./lib/utils");
const { unity } = require("./lib/unity");

runMain(() => {
  const roaringNodeCpp = fs.readFileSync(CPP_UNITY_FILE_PATH, "utf8");

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
        )}: ${CPP_UNITY_FILE_PATH} is outdated or not a production version. Run \`npm run build\` before committing and pushing.`,
      ),
    );
    process.exitCode = 1;
    return;
  }

  try {
    require("../");
  } catch {
    const chalk = require("chalk");
    console.error(
      chalk.redBright(
        `${chalk.underline.bold(
          "ERROR",
        )}: library could not be loaded. Run \`npm run build\` before committing and pushing.`,
      ),
    );
    process.exitCode = 1;
    return;
  }

  execSync("npm run lint", { stdio: "inherit" });
  execSync("npm run test", { stdio: "inherit" });
}, "prepush");
