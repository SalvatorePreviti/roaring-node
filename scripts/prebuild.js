#!/usr/bin/env node

const chalk = require("chalk");

const args = [];
args.push(process.argv[0]);
args.push("-t", "8.0.0");
args.push("-t", "10.0.0");
args.push("-t", "12.0.0");
args.push("-t", "14.0.0");
args.push("-t", "15.0.0");
args.push("-t", "16.0.0");
args.push("-t", "17.0.0");
args.push("-t", "18.0.0");

const token = process.argv[2] || process.env.PREBUILD_GITHUB_TOKEN;

if (token) {
  console.log(`${chalk.yellowBright("WARNING:")} ${chalk.redBright("\npublish binaries enabled\n")}`);

  args.push("-u", token);
} else {
  console.log(chalk.greenBright("\nnot publishing binaries\n"));
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
