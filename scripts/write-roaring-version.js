#!/usr/bin/env node

const fs = require('fs')
const path = require('path')

const root = path.dirname(__dirname)

const roaringVersionLine = fs
  .readFileSync(path.resolve(root, 'src/cpp/CRoaringUnityBuild/roaring.h'), 'utf8')
  .split('\n')
  .find((x) => x.startsWith('#define ROARING_VERSION'))
  .slice('#define ROARING_VERSION'.length)
  .trim()
  .replace(/[^0-9.]/g, '')

if (!/^[0-9]+\.[0-9]+\.[0-9]+$/.test(roaringVersionLine)) {
  throw new Error(`Invalid roaring version ${roaringVersionLine}`)
}

console.log(`Roaring version is ${roaringVersionLine}`)

const oldPackageJson = fs.readFileSync(path.resolve(root, 'package.json'), 'utf8')
const package = JSON.parse(oldPackageJson)
package.roaring_version = roaringVersionLine
const newPackageJson = JSON.stringify(package, null, 2)

if (newPackageJson !== oldPackageJson) {
  fs.writeFileSync(path.resolve(root, 'package.json'), newPackageJson)
  console.log('package.json updated')
}
