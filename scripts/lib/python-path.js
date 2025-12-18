"use strict";

const { spawnSync } = require("node:child_process");
const { join: pathJoin } = require("node:path");
const process = require("node:process");

const detectionSnippet = "import pathlib, sys; print(pathlib.Path(sys.executable).as_posix())";

let pythonPathCache;

function getPythonPath() {
  if (pythonPathCache === undefined) {
    pythonPathCache = resolvePythonPath();
  }
  return pythonPathCache;
}

function resolvePythonPath() {
  const commands = [];
  const arg = process.argv[2];
  if (arg) commands.push(arg);

  const pythonLocation = process.env.pythonLocation || process.env.PYTHON_LOCATION;
  if (pythonLocation) {
    const bin = process.platform === "win32" ? "python.exe" : "bin/python3";
    commands.push(pathJoin(pythonLocation, bin));
  }

  const defaults = process.platform === "win32" ? ["python", "python3", "py"] : ["python3", "python"];
  for (const cmd of defaults) {
    commands.push(cmd);
  }

  const candidates = [];
  for (const cmd of commands) {
    if (cmd && !candidates.includes(cmd)) {
      candidates.push(cmd);
    }
  }

  for (const cmd of candidates) {
    const result = spawnSync(cmd, ["-c", detectionSnippet], { encoding: "utf8" });
    if (result.status === 0 && result.stdout) {
      const value = result.stdout.trim();
      if (value) {
        return value;
      }
    }
  }
  return null;
}

module.exports.getPythonPath = getPythonPath;
