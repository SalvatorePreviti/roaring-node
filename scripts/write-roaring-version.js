#!/usr/bin/env node

const fs = require("fs");
const path = require("path");

const root = path.dirname(__dirname);

const roaringVersionString = fs
  .readFileSync(path.resolve(root, "src/cpp/CRoaringUnityBuild/roaring.h"), "utf8")
  .split("\n")
  .find((x) => x.startsWith("#define ROARING_VERSION"))
  .slice("#define ROARING_VERSION".length)
  .trim()
  .replace(/[^0-9.]/g, "");

if (!/^[0-9]+\.[0-9]+\.[0-9]+$/.test(roaringVersionString)) {
  throw new Error(`Invalid roaring version ${roaringVersionString}`);
}

console.log(`Roaring version is ${roaringVersionString}`);

const oldPackageJson = fs.readFileSync(path.resolve(root, "package.json"), "utf8");
const package = JSON.parse(oldPackageJson);
package.roaring_version = roaringVersionString;
const newPackageJson = `${JSON.stringify(package, null, 2)}\n`;

if (newPackageJson !== oldPackageJson) {
  fs.writeFileSync(path.resolve(root, "package.json"), newPackageJson);
  console.log("package.json updated");
}
