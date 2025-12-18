#!/usr/bin/env node

const { getPythonPath } = require("./lib/python-path");
const { writeFileSync } = require("node:fs");
const process = require("node:process");

function formatPythonPath(pythonPath) {
  if (process.platform === "win32") {
    return pythonPath.replace(/\//g, "\\");
  }
  return pythonPath;
}

function main(pythonPath) {
  if (!pythonPath) {
    throw new Error("Unable to locate a functional Python interpreter.");
  }

  pythonPath = formatPythonPath(pythonPath);

  const lines = ["PYTHON", "NODE_GYP_FORCE_PYTHON"].map((key) => `${key}=${pythonPath}`);
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
