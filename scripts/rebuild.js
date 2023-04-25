#!/usr/bin/env node

const { runMain } = require("./lib/utils");
const { build } = require("./build");

process.argv.push("rebuild");

runMain(build, "rebuild");
