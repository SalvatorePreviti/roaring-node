#!/usr/bin/env node

const { devRunMain, devChildTask, devLog, prettySize } = require("@balsamic/dev");
const path = require("path");
const fs = require("fs");

const ROOT_FOLDER = path.resolve(__dirname, "..");
const SRC_CPP_FOLDER = path.resolve(ROOT_FOLDER, "src/cpp");
const OUTPUT_FILE_PATH = path.resolve(ROOT_FOLDER, "src/roaring-node.cpp");
const INCLUDE_REGEX = /^#\s*include\s+["<](.+)[">]/;
const ROARING_VERSION = /#\s*define\s+ROARING_VERSION\s+"(.+)"/;

const existsCache = new Map();

const exists = (filePath) => {
  let result = existsCache.get(filePath);
  if (result !== undefined) {
    return result;
  }
  result = fs.existsSync(filePath);
  existsCache.set(filePath, result);
  return result;
};

async function unity() {
  const includedFiles = new Set();
  let roaringVersion = null;

  const output = [];

  function processFile(filePath) {
    const content = fs.readFileSync(filePath, "utf8");
    for (const line of content.split("\n")) {
      const includeMatch = line.match(INCLUDE_REGEX);
      if (includeMatch) {
        const includePath = path.resolve(path.dirname(filePath), includeMatch[1]);
        if (exists(includePath)) {
          if (!includedFiles.has(includePath)) {
            output.push(`\n// ${line}\n`);
            includedFiles.add(includePath);
            processFile(includePath);
          }
          continue;
        }
      } else if (!roaringVersion) {
        const match = line.match(ROARING_VERSION);
        if (match) {
          roaringVersion = match[1];
          if (!/^[0-9]+\.[0-9]+\.[0-9]+$/.test(roaringVersion)) {
            throw new Error(`Invalid roaring version ${roaringVersion}`);
          }
        }
      }
      output.push(line);
    }
  }

  processFile(path.resolve(SRC_CPP_FOLDER, "main.cpp"));

  devLog.log();
  devLog.logCyan(`- roaring version ${roaringVersion}`);

  const outputText = output.join("\n");
  devLog.logCyanBright(
    `- ${includedFiles.size} files included. ${prettySize(outputText, { appendBytes: true })} total.`,
  );

  let oldContent;
  try {
    oldContent = fs.readFileSync(OUTPUT_FILE_PATH, "utf8");
  } catch {}
  if (oldContent !== outputText) {
    fs.writeFileSync(path.resolve(OUTPUT_FILE_PATH), outputText, "utf8");
    devLog.logYellow(`- ${path.relative(ROOT_FOLDER, OUTPUT_FILE_PATH)} updated`);
  } else {
    devLog.logBlackBright(`- ${path.relative(ROOT_FOLDER, OUTPUT_FILE_PATH)} is up to date`);
  }

  const packageJsonPath = path.resolve(ROOT_FOLDER, "package.json");
  const oldPackageJson = fs.readFileSync(packageJsonPath, "utf8");
  const package = JSON.parse(oldPackageJson);
  package.roaring_version = roaringVersion;
  const newPackageJson = `${JSON.stringify(package, null, 2)}\n`;

  if (newPackageJson !== oldPackageJson) {
    fs.writeFileSync(packageJsonPath, newPackageJson);
    devLog.logYellow("- package.json updated");
  } else {
    devLog.logBlackBright("- package.json is up to date");
  }
}

async function build() {
  if (exists(SRC_CPP_FOLDER)) {
    await devLog.timed(unity);
    devLog.log();
  }
  await devChildTask.runModuleBin("node-gyp", "node-gyp", [process.argv[2]]);
}

void devRunMain(build);
