#!/usr/bin/env bash

brew update
brew install coreutils scons yasm

scons platform=osx tools=yes target=release

# Create macOS editor DMG image
mkdir artifacts

# This is broken... https://github.com/andreyvit/create-dmg/issues/72
mkdir -p "godot_dmg/"
cp -r "misc/dist/osx_tools.app/" "godot_dmg/Godot.app/"
mkdir -p "godot_dmg/Godot.app/Contents/MacOS/"
cp "bin/godot.osx.opt.tools.64" "godot_dmg/Godot.app/Contents/MacOS/Godot"
git clone "https://github.com/andreyvit/create-dmg.git" --depth=1
(
  cd "create-dmg/"
  ./create-dmg \
  	  --skip-jenkins \
      --volname "Godot" \
      --volicon "../godot_dmg/Godot.app/Contents/Resources/Godot.icns" \
      --hide-extension "Godot.app" \
      "../godot-macos-nightly-x86_64.dmg" \
      "../godot_dmg/"
)
