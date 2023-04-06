# Homebrodot (Godot for Homebrew)

It's a port of the Godot open source game engine to Switch and PlayStation Vita,
via homebrew libraries provided by devkitPro/switchbrew and the VitaSDK team respectively.
See the [original README file](./README-original.md) for info about Godot.

## Releases
See [the GitHub releases page](https://github.com/Stary2001/godot/releases) for pre-built editors for Windows, macOS and Linux.

## How do I export my game?
The editor builds above add a `Switch` exporter to the list - it will generate a .nro and a .pck file that can be transferred to your Switch.
If additional debugging is required, sending the nro via nxlink (make sure to send it to the right place with the -p argument) will allow the output of the console to be viewed on PC.

## How to build?
You shouldn't need to build the engine if you use a release as the templates should be included - these instructions are for development!

[See the official docs](https://docs.godotengine.org/en/latest/development/compiling/)
for compilation instructions for every officially supported platform.

### For Switch:

1. Install [devkitPro pacman](https://devkitpro.org/wiki/devkitPro_pacman)
2. Install the required packages
	```sh
 	# Distributions with pacman preinstalled (like Arch, or Manjaro)
	sudo pacman -S switch-pkg-config switch-freetype switch-bulletphysics switch-libtheora switch-libpcre2 switch-mesa switch-opusfile switch-mbedtls switch-libwebp switch-libvpx switch-miniupnpc switch-libzstd switch-wslay
	# Other distributions (like Debian, Ubuntu, or Windows)
 	sudo dkp-pacman -S switch-pkg-config switch-freetype switch-bulletphysics switch-libtheora switch-libpcre2 switch-mesa switch-opusfile switch-mbedtls switch-libwebp switch-libvpx switch-miniupnpc switch-libzstd switch-wslay
 	```
 3.  Run `scons platform=switch` in the root of the repo.<br>
Add `target=release` to build for release instead of debug.

Then, to build an engine release for Switch,
run `./scripts/create-switch-release.sh` in the root of the repo.

Or to build an export template for the editor,
run `./scripts/create-switch-template.sh` in the root of the repo.

### For Vita:

1. Install [VitaSDK](https://vitasdk.org)
	```sh
 	git clone https://github.com/vitasdk/vdpm
	cd vdpm
	./bootstrap-vitasdk.sh
 	export VITASDK=/usr/local/vitasdk
	export PATH=$VITASDK/bin:$PATH
 	./install-all.sh
 	```
2. Install [PVR_PSP2](https://github.com/GrapheneCt/PVR_PSP2)
	```sh
	git clone https://github.com/isage/vita-packages-extra
	cd vita-packages-extra/pvr_psp2
	vita-makepkg
	vdpm *-arm.tar.xz
	```
3. Run `scons platform=switch` in the root of the repo.<br>
Add `target=release` to build for release instead of debug.

## How can I get help?
Either make an issue on this repo, or join the [Discord](https://discord.gg/yUC3rUk)!

# Credits
* Thanks to devkitPro / switchbrew for producing devkitA64/libnx.
* Extra special thanks to fincs from devkitPro -
The Switch port would have never been possible without their port of the open source nouveau graphics driver.
* cpasjuste for help with development.
* fhidalgosola/utnad for their port / help with development.
