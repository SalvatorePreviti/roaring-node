#!/usr/bin/env node

const { execSync } = require("node:child_process");
const fs = require("node:fs");

const { CPP_UNITY_FILE_PATH, runMain } = require("./lib/utils");
const { unity } = require("./lib/unity");
const colors = require("ansis");

runMain(() => {
  const roaringNodeCpp = fs.readFileSync(CPP_UNITY_FILE_PATH, "utf8");

  const nodeVersion = parseInt(process.versions.node.split(".")[0], 10);
  if (nodeVersion >= 14) {
    execSync("npx lint-staged");
  }

  const unityResult = unity();

  console.log();

  if (unityResult.outputText !== roaringNodeCpp) {
    console.error(
      colors.redBright(
        `${colors.underline.bold(
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
    console.error(
      colors.redBright(
        `${colors.underline.bold(
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
