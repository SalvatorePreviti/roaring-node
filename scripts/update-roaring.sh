#!/bin/bash

set -e

git submodule update --init --recursive

cd submodules/CRoaring

git pull

cd ../..

cd src/cpp/CRoaringUnityBuild

../../submodules/CRoaring/amalgamation.sh
