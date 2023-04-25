#!/bin/bash

set -e

git submodule update --init --recursive

cd submodules/CRoaring

git checkout master
git pull

cd ../..
