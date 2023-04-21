#!/usr/bin/env node

/*
 * This script prebuild and test the library for all supported node versions locally for the local architecture.
 * Of course, it does not work on Windows.
 */

const colors = require("chalk");
const { spawn } = require("child_process");
const { print: printSystemInfo } = require("./system-info");
const fs = require("fs");
const path = require("path");

const supportedNodeVersions = ["12.13.0", "14.13.0", "16.0.0", "18.0.0", "19.0.0", "20.0.0"];

const NATIVE_DIR = path.resolve(__dirname, "../native");
const STAGE_DIR = path.resolve(__dirname, "../build/stage");
const STAGE_TMP_DIR = path.resolve(__dirname, "../.tmp/stage");
const N_DIR = path.resolve(__dirname, "../.tmp/n");
const N_EXECUTABLE_PATH = path.resolve(N_DIR, "node_modules/.bin/n");

function clean() {
  console.log(colors.blueBright("cleaning..."));
  console.time("clean");
  if (fs.existsSync(NATIVE_DIR)) {
    fs.rmdirSync(NATIVE_DIR, { recursive: true });
  }
  if (fs.existsSync(STAGE_DIR)) {
    fs.rmdirSync(STAGE_DIR, { recursive: true });
  }
  if (fs.existsSync(STAGE_TMP_DIR)) {
    fs.rmdirSync(STAGE_TMP_DIR, { recursive: true });
  }
  console.timeEnd("clean");
  console.log();
}

function printUsage() {
  let s = "usage:\n";
  s += "  node scripts/prebuild-local                - build and test\n";
  s += "  node scripts/prebuild-local clean          - cleans the native and build/stage directories\n";
  s += "  node scripts/prebuild-local package        - build and test and package\n";
  s += "  node scripts/prebuild-local deploy [token] - build, test, package and deploy\n";
  s += "\n";
  console.log(colors.yellow(s));
}

async function main() {
  const command = process.argv[2];

  console.log(colors.magentaBright(__filename, " ", command || ""));

  if (command === "clean") {
    clean();
    return;
  }

  const isDeploy = command === "deploy";
  const isPackage = isDeploy || command === "package" || command === "pack";

  let NODE_PRE_GYP_GITHUB_TOKEN;
  if (isDeploy) {
    NODE_PRE_GYP_GITHUB_TOKEN = process.argv[3] || process.env.NODE_PRE_GYP_GITHUB_TOKEN;
    if (!NODE_PRE_GYP_GITHUB_TOKEN) {
      try {
        NODE_PRE_GYP_GITHUB_TOKEN = fs
          .readFileSync(path.resolve(__dirname, "../.github-key"), "utf8")
          .replace("\n", "")
          .replace("\r", "")
          .trim();
      } catch (e) {}
    }
    if (!NODE_PRE_GYP_GITHUB_TOKEN) {
      console.log(colors.redBright("\nError: NODE_PRE_GYP_GITHUB_TOKEN environment variable not found"));
      console.log();
      printUsage();
      process.exitCode = 2;
      return;
    }
  }

  if (isPackage) {
    await spawnAsync("node", [require.resolve("./build.js"), "--no-compile"]);
  }

  if (command && !isDeploy && !isPackage) {
    printUsage();
    process.exitCode = 1;
    return;
  }

  clean();

  console.time("prebuild");
  console.log(colors.cyanBright(`* Building for node ${supportedNodeVersions.join(" ")}\n`));

  printSystemInfo();

  console.time("installing n");
  fs.mkdirSync(N_DIR, { recursive: true });
  fs.writeFileSync(
    path.resolve(N_DIR, "package.json"),
    JSON.stringify({ name: "n-dir", dependencies: { n: "latest" } }),
  );
  await spawnAsync("npm", ["install", "--ignore-scripts", "--no-audit", "--no-save", "--install-strategy", "shallow"], {
    cwd: N_DIR,
  });
  console.timeEnd("installing n");

  console.time("installing node versions");
  await Promise.all(
    supportedNodeVersions.map((nodeVersion) => spawnAsync(N_EXECUTABLE_PATH, ["i", nodeVersion, "--download"])),
  );
  console.timeEnd("installing node versions");

  const testingPromises = [];

  console.time("building");
  for (const nodeVersion of supportedNodeVersions) {
    console.log(colors.blueBright(`\nbuilding node ${nodeVersion}\n`));
    const args = ["exec", nodeVersion, "npx", "node-pre-gyp", "--release", "rebuild"];
    if (isPackage) {
      args.push("package");
    }
    await spawnAsync(N_EXECUTABLE_PATH, args);

    if (isPackage) {
      // Move stage folder to a temporary folder so it does not get clean by node-gyp
      await mergeDirs(STAGE_DIR, STAGE_TMP_DIR);
    }

    testingPromises.push(spawnAsync(N_EXECUTABLE_PATH, ["run", nodeVersion, require.resolve("./test.js")]));
  }
  console.timeEnd("building");

  await Promise.all(testingPromises);

  console.timeEnd("prebuild");

  if (isPackage) {
    await mergeDirs(STAGE_TMP_DIR, STAGE_DIR);
    if (fs.existsSync(STAGE_TMP_DIR)) {
      fs.rmdirSync(STAGE_TMP_DIR, { recursive: true });
    }
  }

  if (isDeploy) {
    if (isDeploy && process.argv[3]) {
      process.env.NODE_PRE_GYP_GITHUB_TOKEN = process.argv[3];
    }

    await spawnAsync("npx", ["node-pre-gyp-github", "publish"], {
      env: { ...process.env, NODE_PRE_GYP_GITHUB_TOKEN },
    });
  }

  console.log(colors.greenBright("\nok\n"));
}

function spawnAsync(command, args, options) {
  return new Promise((resolve, reject) => {
    const npx = spawn(command, args, { stdio: "inherit", ...options });
    npx.on("error", (e) => {
      reject(e);
    });
    npx.on("close", (code) => {
      if (code === 0) {
        resolve();
      } else {
        reject(new Error(`npx exited with code ${code}`));
      }
    });
  });
}

async function mergeDirs(src, dest) {
  const promises = [];
  await fs.promises.mkdir(dest, { recursive: true });
  for (const file of fs.readdirSync(src)) {
    const srcFile = path.resolve(src, file);
    const destFile = path.resolve(dest, file);
    const isDir = (await fs.promises.lstat(srcFile)).isDirectory();
    if (isDir) {
      await mergeDirs(srcFile, destFile);
    } else {
      if (fs.existsSync(destFile)) {
        await fs.promises.rm(destFile, { recursive: true });
      }
      promises.push(fs.promises.rename(srcFile, destFile));
    }
  }
  return Promise.all(promises);
}

main().catch((error) => {
  console.error(error);
});
