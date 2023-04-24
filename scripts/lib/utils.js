const colors = require("chalk");
const util = require("util");
const fs = require("fs");
const path = require("path");
const { spawn } = require("child_process");

const ROOT_FOLDER = path.resolve(__dirname, "../../");
const CPP_SRC_FOLDER_PATH = path.resolve(ROOT_FOLDER, "src/cpp");
const CPP_UNITY_FILE_PATH = path.resolve(ROOT_FOLDER, "roaring-node.cpp");

let _prebuildYaml;

function getPrebuildYaml() {
  return (
    _prebuildYaml ||
    (_prebuildYaml = require("yaml").parse(
      fs.readFileSync(path.resolve(ROOT_FOLDER, ".github/workflows/prebuild.yml"), "utf-8"),
    ))
  );
}

let BINARY_OUTPUT_FILE_PATH;

function getBinaryOutputFilePath() {
  if (!BINARY_OUTPUT_FILE_PATH) {
    BINARY_OUTPUT_FILE_PATH = require("@mapbox/node-pre-gyp/lib/pre-binding").find(
      path.join(ROOT_FOLDER, "package.json"),
    );
    if (!fs.existsSync(BINARY_OUTPUT_FILE_PATH)) {
      BINARY_OUTPUT_FILE_PATH = path.resolve(ROOT_FOLDER, "build", "Release", "roaring.node");
    }
  }
}

module.exports = {
  ROOT_FOLDER,
  CPP_SRC_FOLDER_PATH,
  CPP_UNITY_FILE_PATH,

  getPrebuildYaml,
  getBinaryOutputFilePath,

  runMain,
  spawnAsync,
  mergeDirs,
};

function runMain(fn, title) {
  if (title) {
    console.log(colors.blueBright(`\n⬢ ${colors.cyanBright(title)}\n`));
  }

  if (!fn.name) {
    Reflect.defineProperty(fn, "name", {
      value: "main",
      configurable: true,
      enumerable: false,
      writable: false,
    });
  }

  const totalTime = () => (title ? `${colors.cyan(title)}` : "") + colors.italic(` ⌚ ${process.uptime().toFixed(2)}s`);

  let _processCompleted = false;

  const processCompleted = () => {
    if (!process.exitCode) {
      console.log(colors.greenBright("✅ OK"), totalTime());
    } else {
      console.log(colors.redBright(`❌ Failed: exitCode${process.exitCode}`), totalTime());
    }
    console.log();
  };

  process.on("exit", () => {
    if (!_processCompleted) {
      _processCompleted = true;
      processCompleted();
    }
  });

  const handleMainError = (e) => {
    if (!process.exitCode) {
      process.exitCode = 1;
    }
    console.log("❌ ", colors.redBright(util.inspect(e, { colors: colors.level > 0 })));
    console.log(totalTime());
    console.log();
  };

  try {
    const result = fn();
    if (typeof result === "object" && result && typeof result.then === "function") {
      result.then(() => {}, handleMainError);
    }
  } catch (e) {
    handleMainError(e);
  }
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
