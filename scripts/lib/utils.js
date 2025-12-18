const colors = require("ansis");
const util = require("node:util");
const fs = require("node:fs");
const path = require("node:path");
const { spawn, fork } = require("node:child_process");

const ROOT_FOLDER = path.resolve(__dirname, "../../");
const CPP_SRC_FOLDER_PATH = path.resolve(ROOT_FOLDER, "src/cpp");
const CPP_UNITY_FILE_PATH = path.resolve(ROOT_FOLDER, "roaring-node.cpp");

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
  return BINARY_OUTPUT_FILE_PATH;
}

module.exports = {
  ROOT_FOLDER,
  CPP_SRC_FOLDER_PATH,
  CPP_UNITY_FILE_PATH,

  getBinaryOutputFilePath,

  runMain,
  spawnAsync,
  forkAsync,
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
    const childProcess = spawn(command, args, { stdio: "inherit", ...options });
    childProcess.on("error", (e) => {
      reject(e);
    });
    childProcess.on("close", (code) => {
      if (code === 0) {
        resolve();
      } else {
        reject(new Error(`${command} exited with code ${code}`));
      }
    });
  });
}

function forkAsync(modulePath, args, options) {
  return new Promise((resolve, reject) => {
    const childProcess = fork(modulePath, args, { stdio: "inherit", ...options });
    childProcess.on("error", (e) => {
      reject(e);
    });
    childProcess.on("close", (code) => {
      if (code === 0) {
        resolve();
      } else {
        reject(new Error(`${modulePath} exited with code ${code}`));
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
