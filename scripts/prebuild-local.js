#!/usr/bin/env node

/*
 * This script prebuild and test the library for all supported node versions locally for the local architecture.
 * Of course, it does not work on Windows.
 */

const colors = require("chalk");
const { print: printSystemInfo } = require("./system-info");
const fs = require("fs");
const path = require("path");

const { spawnAsync, mergeDirs, runMain } = require("./lib/utils");

const { startPublishAssets } = require("./node-pre-gyp-publish");

const supportedNodeVersions = ["12.13.0", "14.13.0", "16.1.0", "18.1.0", "20.0.0"];

const NATIVE_DIR = path.resolve(__dirname, "../native");
const STAGE_DIR = path.resolve(__dirname, "../build/stage");
const STAGE_TMP_DIR = path.resolve(__dirname, "../.tmp/stage");
const TOOLS_DIR = path.resolve(__dirname, "../.tmp/tools");
const N_EXECUTABLE_PATH = path.resolve(TOOLS_DIR, "node_modules/.bin/n");

function clean() {
  console.log(colors.blueBright("cleaning..."));
  if (fs.existsSync(NATIVE_DIR)) {
    fs.rmSync(NATIVE_DIR, { recursive: true });
  }
  if (fs.existsSync(STAGE_DIR)) {
    fs.rmSync(STAGE_DIR, { recursive: true });
  }
  if (fs.existsSync(STAGE_TMP_DIR)) {
    fs.rmSync(STAGE_TMP_DIR, { recursive: true });
  }
  console.log();
}

function printUsage() {
  let s = "usage:\n";
  s += "  node scripts/prebuild-local         - build and test\n";
  s += "  node scripts/prebuild-local clean   - cleans the native and build/stage directories\n";
  s += "  node scripts/prebuild-local package - build and test and package\n";
  s += "  node scripts/prebuild-local deploy  - build, test, package and deploy\n";
  s += "\n";
  console.log(colors.yellow(s));
}

async function main() {
  const command = process.argv[2];

  if (process.argv.includes("--dev")) {
    throw new Error("Invalida argument --dev");
  }
  if (process.argv.includes("--no-compile")) {
    throw new Error("Invalida argument --no-compile");
  }

  console.log(colors.magentaBright("command: ", colors.italic(command) || ""));
  console.log();

  if (command === "clean") {
    clean();
    return;
  }

  const isDeploy = command === "deploy";
  const isPackage = isDeploy || command === "package" || command === "pack";

  const publisher = isDeploy ? await startPublishAssets() : undefined;

  if (isDeploy) {
    publisher.checkGithubKey();
  }

  if (isPackage) {
    await spawnAsync("node", [require.resolve("./rebuild.js"), "--no-compile"]);
  }

  if (command && !isDeploy && !isPackage) {
    printUsage();
    process.exitCode = 1;
    return;
  }

  clean();

  console.log(colors.cyanBright(`* Building for node ${supportedNodeVersions.join(" ")}\n`));

  printSystemInfo();

  console.log(colors.blueBright("- installing tools"));
  console.time("installing tools");
  fs.mkdirSync(TOOLS_DIR, { recursive: true });
  fs.writeFileSync(
    path.resolve(TOOLS_DIR, "package.json"),
    JSON.stringify({ name: "tools", private: true, dependencies: { n: "latest", "node-gyp": "latest" } }),
  );
  await spawnAsync("npm", ["install", "--ignore-scripts", "--no-audit", "--no-save"], {
    cwd: TOOLS_DIR,
  });
  process.env.npm_config_node_gyp = path.resolve(TOOLS_DIR, "node_modules/node-gyp/bin/node-gyp.js");
  if (!fs.existsSync(process.env.npm_config_node_gyp)) {
    process.env.npm_config_node_gyp = require.resolve("node-gyp/bin/node-gyp.js");
  }
  console.timeEnd("installing tools");

  console.log(colors.blueBright("- installing node versions"));
  console.time("installing node versions");
  await Promise.all(
    supportedNodeVersions.map((nodeVersion) => spawnAsync(N_EXECUTABLE_PATH, ["i", nodeVersion, "--download"])),
  );
  console.timeEnd("installing node versions");

  console.log(colors.blueBright("- building"));
  console.time("building");
  let nodeVersionIndex = 0;
  for (const nodeVersion of supportedNodeVersions) {
    console.log(colors.blueBright(`\n- building for node ${nodeVersion}\n`));
    const args = ["exec", nodeVersion, "npx", "node-pre-gyp", "--release", "rebuild"];
    if (isPackage) {
      args.push("package");
    }
    await spawnAsync(N_EXECUTABLE_PATH, args);

    console.log(colors.blueBright("- testing"));
    const testArgs = ["run", nodeVersion, require.resolve("./test.js")];
    if (nodeVersionIndex > 0) {
      testArgs.push("--notypecheck");
    }
    await spawnAsync(N_EXECUTABLE_PATH, testArgs);

    if (isDeploy) {
      console.log(colors.blueBright("- uploading"));
      await publisher.upload();
    }

    if (isPackage) {
      // Move stage folder to a temporary folder so it does not get clean by node-gyp
      await mergeDirs(STAGE_DIR, STAGE_TMP_DIR);
    }

    ++nodeVersionIndex;
  }
  console.timeEnd("building");

  if (isPackage) {
    // Move the directory back to build/stage
    await mergeDirs(STAGE_TMP_DIR, STAGE_DIR);
    if (fs.existsSync(STAGE_TMP_DIR)) {
      fs.rmdirSync(STAGE_TMP_DIR, { recursive: true });
    }
  }
}

runMain(main, "prebuild-local");
