#!/bin/bash

set -e

git submodule update --init --recursive

cd submodules/CRoaring

cd ../..

mkdir -p src/cpp/CRoaringUnityBuild

cd src/cpp/CRoaringUnityBuild

../../../submodules/CRoaring/amalgamation.sh

rm amalgamation_demo.c
rm amalgamation_demo.cpp
rm roaring.hh

cd ../../..
