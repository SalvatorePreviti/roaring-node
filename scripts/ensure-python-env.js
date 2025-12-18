#!/usr/bin/env node

const { getPythonPath } = require("./lib/python-path");
const { writeFileSync } = require("node:fs");
const process = require("node:process");

function main(pythonPath) {
  const lines = ["PYTHON", "NODE_GYP_FORCE_PYTHON", "npm_config_python"].map((key) => `${key}=${pythonPath}`);
  const text = `${lines.join("\n")}\n`;
  const envFile = process.env.GITHUB_ENV;
  if (!envFile) {
    console.log(text);
    return;
  }

  writeFileSync(envFile, text, { flag: "a" });
  console.log(`Configured node-gyp python -> ${pythonPath}`);
}

main(getPythonPath());
