#!/bin/bash

set -e

git submodule update --init --recursive

cd submodules/CRoaring

git checkout master
git pull

cd ../..

mkdir -p src/cpp/CRoaringUnityBuild

cd src/cpp/CRoaringUnityBuild

../../../submodules/CRoaring/amalgamation.sh

rm amalgamation_demo.c
rm amalgamation_demo.cpp
rm roaring.hh

cd ../../..

node scripts/fix-roaring-amalgamation.js
node scripts/write-roaring-version.js
