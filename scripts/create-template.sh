#!/usr/bin/env bash

sudo apt-get update
sudo apt-get -y install zip

mkdir templates
cp switch_release_debug.nro templates/switch_release.nro
cp platform/switch/romfs/applet_splash.rgba.gz templates/switch_applet_splash.rgba.gz
zip -r godot_templates_switch_only.tpz templates
