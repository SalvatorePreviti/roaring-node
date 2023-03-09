#!/usr/bin/env node

const colors = require("chalk");

function prebuild() {
  const args = [];
  args.push(process.argv[0]);
  args.push("-t", "8.10.0");
  args.push("-t", "10.0.0");
  args.push("-t", "12.0.0");
  args.push("-t", "14.0.0");
  args.push("-t", "15.0.0");
  args.push("-t", "16.0.0");
  args.push("-t", "18.0.0");

  const token = process.argv[2] || process.env.PREBUILD_GITHUB_TOKEN;

  if (token) {
    console.log(`${colors.yellowBright("WARNING:")} ${colors.redBright("\npublish binaries enabled\n")}`);

    args.push("-u", token);
  } else {
    console.log(colors.greenBright("\nnot publishing binaries\n"));
  }

  for (let i = 2; i < process.argv.length; ++i) {
    args.push(process.argv[i]);
  }

  const oldProcessArgv = process.argv;

  process.argv = args;

  try {
    require("prebuild/bin");
  } finally {
    process.argv = oldProcessArgv;
  }
}

console.time("prebuild");
prebuild();
console.timeEnd("prebuild");
