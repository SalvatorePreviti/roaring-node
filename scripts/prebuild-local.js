#!/usr/bin/env node

/*
 * This script prebuild and test the library for all supported node versions locally for the local architecture.
 * Of course, it does not work on Windows.
 */

const colors = require("ansis");
const { printSystemInfo } = require("./system-info");
const fs = require("node:fs");
const path = require("node:path");

const { spawnAsync, mergeDirs, runMain, ROOT_FOLDER, forkAsync, NPM_COMMAND } = require("./lib/utils");

const { startPublishAssets } = require("./node-pre-gyp-publish");

const NODE_VERSIONS = ["20.9.0", "22.8.0", "24.5.0", "25.0.0"];

const NATIVE_DIR = path.resolve(ROOT_FOLDER, "native");
const STAGE_DIR = path.resolve(ROOT_FOLDER, "build/stage");
const STAGE_TMP_DIR = path.resolve(ROOT_FOLDER, ".tmp/stage");
const TOOLS_DIR = path.resolve(ROOT_FOLDER, ".tmp/tools");
const N_EXECUTABLE_PATH = path.resolve(TOOLS_DIR, "node_modules/.bin/n");

const rmdir = fs.promises.rm || fs.promises.rmdir;

async function clean() {
  console.log(colors.blueBright("cleaning..."));
  const promises = [];
  if (fs.existsSync(NATIVE_DIR)) {
    promises.push(rmdir(NATIVE_DIR, { recursive: true }));
  }
  if (fs.existsSync(STAGE_DIR)) {
    promises.push(rmdir(STAGE_DIR, { recursive: true }));
  }
  if (fs.existsSync(STAGE_TMP_DIR)) {
    promises.push(rmdir(STAGE_TMP_DIR, { recursive: true }));
  }
  await Promise.all(promises);
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
    throw new Error("Invalid argument --dev");
  }
  if (process.argv.includes("--no-compile")) {
    throw new Error("Invalid argument --no-compile");
  }

  console.log(colors.magentaBright("command: ", colors.italic(command) || ""));
  console.log();

  if (command === "clean") {
    await clean();
    return;
  }

  const isDeploy = command === "deploy";
  const ensureVersionFlag = process.argv.includes("--ensure-version");
  const isEnsureCommand = command === "ensure-version";
  const isPackage = isDeploy || command === "package" || command === "pack";

  const publisher = isDeploy ? await startPublishAssets() : undefined;

  if (isDeploy || isEnsureCommand) {
    publisher.checkGithubKey();
  }

  if (isPackage) {
    await forkAsync(require.resolve("./build.js"), ["--no-compile"]);
  }

  if (command && !isDeploy && !isPackage) {
    printUsage();
    process.exitCode = 1;
    return;
  }

  await clean();

  console.log(colors.cyanBright(`* Building for node ${NODE_VERSIONS.join(" ")}\n`));

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
    NODE_VERSIONS.map((nodeVersion) => spawnAsync(N_EXECUTABLE_PATH, ["i", nodeVersion, "--download"])),
  );
  console.timeEnd("installing node versions");

  console.log(colors.blueBright("- building"));
  console.time("building");
  for (const nodeVersion of NODE_VERSIONS) {
    console.log(colors.blueBright(`\n- building for node ${nodeVersion}\n`));
    const args = ["run", nodeVersion, path.resolve(ROOT_FOLDER, "node-pre-gyp.js"), "rebuild"];
    if (isPackage) {
      args.push("package", "testpackage");
    }
    await spawnAsync(N_EXECUTABLE_PATH, args);

    console.log(colors.blueBright("- testing"));
    const testArgs = ["run", nodeVersion, NPM_COMMAND, "run", "test"];
    await spawnAsync(N_EXECUTABLE_PATH, testArgs);

    if (isDeploy) {
      console.log(colors.blueBright("- uploading"));
      await publisher.upload();
    }

    if (isPackage) {
      // Move stage folder to a temporary folder so it does not get clean by node-gyp
      await mergeDirs(STAGE_DIR, STAGE_TMP_DIR);
    }
  }
  console.timeEnd("building");

  if (isPackage) {
    // Move the directory back to build/stage
    await mergeDirs(STAGE_TMP_DIR, STAGE_DIR);
    if (fs.existsSync(STAGE_TMP_DIR)) {
      await rmdir(STAGE_TMP_DIR, { recursive: true });
    }
  }

  // If invoked only to ensure the release/tag exists, do it now and exit.
  if (ensureVersionFlag || isEnsureCommand) {
    if (!publisher) {
      console.log(
        colors.yellow(
          "No publisher available to ensure version. Are you running with 'deploy' or 'ensure-version' command?",
        ),
      );
      process.exitCode = 1;
      return;
    }
    const ok = await publisher.ensureRelease();
    if (ok) {
      console.log(colors.greenBright(`Release ${require("../package.json").version} exists or was created.`));
      return;
    }
    console.log(
      colors.yellowBright(
        `Release ${require("../package.json").version} could not be ensured (branch or authentication?).`,
      ),
    );
    process.exitCode = 1;
  }
}

runMain(main, "prebuild-local");
