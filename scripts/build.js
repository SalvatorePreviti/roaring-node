#!/usr/bin/env node

const colors = require("chalk");
const path = require("path");
const fs = require("fs");
const { execSync } = require("child_process");
const { runMain } = require("./lib/utils");

const { CPP_UNITY_FILE_PATH, ROOT_FOLDER, getBinaryOutputFilePath } = require("./lib/utils");
const { unity } = require("./lib/unity");

async function development() {
  console.log();
  const warningMessage = "Development mode is enabled. Rebuild for production before publishing.";
  console.warn(colors.yellowBright.underline.bold("WARNING") + colors.yellow(`: ${warningMessage}`));
  console.log();
  let outputText = "";
  outputText += "#define ROARING_NODE_DEV 1\n\n";
  outputText += `#pragma message (${JSON.stringify(warningMessage)})\n`;
  outputText += '#include "src/cpp/main.cpp"\n';

  let oldContent;
  try {
    oldContent = fs.readFileSync(CPP_UNITY_FILE_PATH, "utf8");
  } catch {}
  if (oldContent !== outputText) {
    fs.writeFileSync(path.resolve(CPP_UNITY_FILE_PATH), outputText, "utf8");
    console.log(colors.yellow(`- ${path.relative(ROOT_FOLDER, CPP_UNITY_FILE_PATH)} updated`));
  } else {
    console.log(colors.blackBright(`- ${path.relative(ROOT_FOLDER, CPP_UNITY_FILE_PATH)} is up to date`));
  }
}

async function build(buildMode) {
  if (buildMode !== "build" && buildMode !== "rebuild") {
    buildMode = process.argv.slice(2).includes("rebuild") ? "rebuild" : "build";
  }

  if (
    process.argv.includes("dev") ||
    process.argv.includes("--dev") ||
    process.argv.includes("development") ||
    process.argv.includes("--development")
  ) {
    console.time("Development mode");
    await development();
    console.timeEnd("Development mode");
  } else {
    console.time("Unity build");
    const unityResult = unity();

    let oldContent;
    try {
      oldContent = fs.readFileSync(CPP_UNITY_FILE_PATH, "utf8");
    } catch {}
    if (oldContent !== unityResult.outputText) {
      fs.writeFileSync(path.resolve(CPP_UNITY_FILE_PATH), unityResult.outputText, "utf8");
      console.log(colors.yellow(`- ${path.relative(ROOT_FOLDER, CPP_UNITY_FILE_PATH)} updated`));
    } else {
      console.log(colors.blackBright(`- ${path.relative(ROOT_FOLDER, CPP_UNITY_FILE_PATH)} is up to date`));
    }

    const packageJsonPath = path.resolve(ROOT_FOLDER, "package.json");
    const oldPackageJson = fs.readFileSync(packageJsonPath, "utf8");
    const package = JSON.parse(oldPackageJson);
    package.roaring_version = unityResult.roaringVersion;
    const newPackageJson = `${JSON.stringify(package, null, 2)}\n`;

    if (newPackageJson !== oldPackageJson) {
      fs.writeFileSync(packageJsonPath, newPackageJson);
      console.log(colors.yellow("- package.json updated"));
    } else {
      console.log(colors.blackBright("- package.json is up to date"));
    }

    console.timeEnd("Unity build");
  }
  console.log();

  process.on("exit", () => {
    console.log();

    const BINARY_OUTPUT_FILE_PATH = getBinaryOutputFilePath();
    if (fs.existsSync(BINARY_OUTPUT_FILE_PATH)) {
      console.log();
      console.log(
        colors.green(
          `* ${path.relative(ROOT_FOLDER, BINARY_OUTPUT_FILE_PATH)} compiled. ${
            fs.statSync(BINARY_OUTPUT_FILE_PATH).size
          } bytes`,
        ),
      );
    }
    console.log();
  });

  if (!process.argv.includes("--no-compile")) {
    execSync(`node ${path.resolve(ROOT_FOLDER, "node-pre-gyp.js")} ${buildMode}`, {
      stdio: "inherit",
      env: process.env,
    });
    execSync(`node ${require.resolve("./test.js")}`, { stdio: "inherit", env: process.env });
  }
}

module.exports.build = build;

if (require.main === module) {
  const buildMode = process.argv.slice(2).includes("rebuild") ? "rebuild" : "build";
  runMain(build, buildMode);
}
