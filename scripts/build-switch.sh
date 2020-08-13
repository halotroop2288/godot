#!/usr/bin/env bash

sudo apt-get update
sudo apt-get -y install scons
scons platform=switch tools=no target=release_debug -j2