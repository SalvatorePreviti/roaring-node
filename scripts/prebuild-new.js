#!/usr/bin/env node

const { execSync } = require("child_process");
const path = require("path");
const fs = require("fs");

const supportedNodeVersions = ["8.10.0", "10.0.0", "12.0.0", "14.0.0", "16.0.0", "18.0.0", "20.0.0"];

// Install the given node version in a temporary folder
function installNodeVersion(nodeVersion) {
  const tempDir = fs.mkdtempSync(path.join(__dirname, "../node/node-"));
  const nodeDir = path.join(tempDir, "node");
  const nodeBinDir = path.join(nodeDir, "bin");

  execSync(`curl -sL https://nodejs.org/dist/v${nodeVersion}/node-v${nodeVersion}-linux-x64.tar.xz | tar xJ`, {
    cwd: tempDir,
  });

  return nodeBinDir;
}

function prebuild() {
  // For each node version, install it, build the binary, run the tests
  for (const nodeVersion of supportedNodeVersions) {
    const nodeBinDir = installNodeVersion(nodeVersion);

    const oldPath = process.env.PATH;
    process.env.PATH = `${nodeBinDir}:${oldPath}`;

    try {
      execSync("npm run recompile", { stdio: "inherit" });
      execSync("npm run test", { stdio: "inherit" });
    } finally {
      process.env.PATH = oldPath;
    }
  }
}

prebuild();
