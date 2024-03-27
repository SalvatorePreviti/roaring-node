#!/usr/bin/env node

const colors = require("ansis");
const os = require("os");

function getSystemInfo() {
  const cpus = os.cpus();
  let physicalCpuCount;

  try {
    physicalCpuCount = require("physical-cpu-count");
  } catch (e) {
    // Ignore error
  }
  if (!physicalCpuCount) {
    physicalCpuCount = Math.max(1, cpus.length / 2);
  }

  const systemInfo = {
    physicalCpuCount,
  };

  systemInfo.print = function print() {
    console.log();
    console.log(
      colors.whiteBright("Platform"),
      ":",
      colors.cyanBright(os.type()),
      colors.greenBright(os.release()),
      colors.yellowBright(process.arch),
    );
    console.log(colors.whiteBright("CPU     "), ":", colors.cyanBright(cpus[0].model));
    console.log(
      colors.whiteBright("Cores   "),
      ":",
      colors.cyanBright(physicalCpuCount),
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
  };

  return systemInfo;
}

module.exports = getSystemInfo();

if (require.main === module) {
  module.exports.print();
}
