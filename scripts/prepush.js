#!/usr/bin/env node

const { execSync } = require("child_process");
const fs = require("fs");

const { CPP_UNITY_FILE_PATH, runMain } = require("./lib/utils");
const { unity } = require("./lib/unity");

runMain(() => {
  const roaringNodeCpp = fs.readFileSync(CPP_UNITY_FILE_PATH, "utf8");

  execSync("npx lint-staged");

  const unityResult = unity();

  console.log();

  if (unityResult.outputText !== roaringNodeCpp) {
    const chalk = require("ansis");
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
    const chalk = require("ansis");
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
  execSync("npx vitest --run", { stdio: "inherit" });
}, "prepush");
