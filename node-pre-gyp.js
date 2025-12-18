/* eslint-disable no-console */
"use strict";

const { fork } = require("node:child_process");
const fs = require("node:fs");
const path = require("node:path");

function resolveNodeGypPath() {
  const fs = require("node:fs");
  const path = require("node:path");

  const seen = new Set();
  const addCandidates = (list) => {
    for (const item of list) {
      if (item && !seen.has(item)) {
        seen.add(path.resolve(item));
      }
    }
  };

  try {
    addCandidates([require.resolve("node-gyp/bin/node-gyp.js")]);
  } catch {}

  const cwd = process.cwd();
  addCandidates([
    path.join(cwd, "node_modules", "node-gyp", "bin", "node-gyp.js"),
    path.join(cwd, "node_modules", "npm", "node_modules", "node-gyp", "bin", "node-gyp.js"),
  ]);

  const execDir = path.dirname(process.execPath);
  const roots = [
    path.join(execDir, "..", "lib", "node_modules", "npm"),
    path.join(execDir, "..", "node_modules", "npm"),
    path.join(execDir, "node_modules", "npm"),
  ];
  for (const root of roots) {
    addCandidates([path.join(root, "node_modules", "node-gyp", "bin", "node-gyp.js")]);
  }

  for (const candidate of seen) {
    if (fs.existsSync(candidate)) {
      return candidate;
    }
  }
  return null;
}

const ROARING_NODE_PRE_GYP = process.env.ROARING_NODE_PRE_GYP;
if (ROARING_NODE_PRE_GYP) {
  process.env.ROARING_NODE_PRE_GYP = "";
}

function roaring32NodePreGyp() {
  console.time("node-pre-gyp");
  process.on("exit", () => {
    console.timeEnd("node-pre-gyp");
  });

  process.chdir(__dirname);

  if (ROARING_NODE_PRE_GYP !== "custom-rebuild") {
    const nodeGypPath = resolveNodeGypPath();
    if (nodeGypPath) {
      process.env.npm_config_node_gyp = nodeGypPath;
    }

    require("@mapbox/node-pre-gyp/lib/main");
  } else {
    const forkAsync = (modulePath, args, options) => {
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
    };

    const main = async () => {
      console.log("* rebuild...");

      let glibc;
      try {
        // eslint-disable-next-line node/no-unsupported-features/node-builtins
        const header = process.report.getReport().header;
        glibc = header.glibcVersionRuntime;
      } catch {}

      if (typeof glibc !== "string" || !glibc || glibc === "unknown") {
        glibc = null;
      } else {
        glibc = glibc.trim();
      }

      console.log("versions:", {
        node: process.version,
        v8: process.versions.v8,
        glibc: glibc || null,
      });

      console.time("rebuild");
      const rebuildArgs = ["rebuild"];
      await forkAsync(__filename, rebuildArgs);
      console.log();
      console.timeEnd("rebuild");
      console.log();

      // Clean debug info and temporary files
      for (let d of fs.readdirSync("native")) {
        d = path.resolve(__dirname, "native", d);
        if (fs.lstatSync(d).isDirectory()) {
          for (let f of fs.readdirSync(d)) {
            f = path.resolve(d, f);
            for (const ext of [".ipdb", ".iobj", ".pdb", ".obj", ".lib", ".exp", ".tmp"]) {
              if (f.endsWith(ext)) {
                console.log(`- removing file ${f}`);
                fs.unlinkSync(f);
              }
            }
          }
        }
      }

      console.log();
      console.log("* packaging...");
      console.time("packaging");
      const packageArgs = ["package", "testpackage"];
      await forkAsync(__filename, packageArgs);
      console.timeEnd("packaging");
      console.log();
    };

    main().catch((e) => {
      console.error(e);
      if (!process.exitCode) {
        process.exitCode = 1;
      }
    });
  }
}

if (ROARING_NODE_PRE_GYP !== "false") {
  roaring32NodePreGyp();
}
