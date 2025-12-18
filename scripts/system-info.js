#!/usr/bin/env node

Object.defineProperty(exports, "__esModule", { value: true });

const colors = require("ansis");
const os = require("node:os");

let systemInfoCache = null;

function getSystemInfo() {
  if (systemInfoCache) {
    return systemInfoCache;
  }

  let physicalCpuCount;

  try {
    physicalCpuCount = require("physical-cpu-count");
  } catch (_e) {
    // Ignore error
  }
  if (!physicalCpuCount) {
    const cpus = os.cpus();
    physicalCpuCount = Math.max(1, cpus.length / 2);
  }

  systemInfoCache = { physicalCpuCount };

  return systemInfoCache;
}

function printSystemInfo() {
  console.log();
  console.log(
    colors.whiteBright("Platform"),
    ":",
    colors.cyanBright(os.type()),
    colors.greenBright(os.release()),
    colors.yellowBright(process.arch),
  );
  const cpus = os.cpus();
  console.log(colors.whiteBright("CPU     "), ":", colors.cyanBright(cpus[0].model));
  console.log(
    colors.whiteBright("Cores   "),
    ":",
    colors.cyanBright(getSystemInfo().physicalCpuCount),
    colors.cyan("physical"),
    "-",
    colors.greenBright(cpus.length),
    colors.green("logical"),
  );
  console.log(
    colors.whiteBright("Memory  "),
    ":",
    colors.cyanBright((os.totalmem() / 1073741824).toFixed(2)),
    colors.cyan("GB"),
  );
  console.log(
    colors.whiteBright("NodeJS  "),
    ":",
    colors.cyanBright(process.version),
    "-",
    colors.green("V8"),
    colors.greenBright(`v${process.versions.v8}`),
  );
  console.log();
}

module.exports.getSystemInfo = getSystemInfo;
module.exports.printSystemInfo = printSystemInfo;

if (require.main === module) {
  printSystemInfo();
}
