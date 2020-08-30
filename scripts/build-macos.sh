#!/usr/bin/env bash

brew update
brew install coreutils scons yasm

scons platform=osx tools=yes target=release_debug -j2
