# Godot Engine 4 for 3DS

## What is it?

This repository contains an unfinished port of the Godot 4 game engine for the Nintendo 3DS.

The goal is to be able to develop 3DS games using the Godot Engine.

The renderer is not yet functional, however, the engine does start and run gdscript code headless.

## How does this work

The godot runtime gets bundled in a 3DSX with the content romfs directory present in platform/3ds/romfs.

Godot, when starting uses the arguments `--main-pack romfs:/Game.pck --display-driver 3ds --audio-driver Dummy`

As such, if you want to change the project loaded by godot on startup, you will need to change the Game.pck that is bundled in the romfs directory.

## Is this legal?

Only open source SDKs are used for the development of this port.

No Nintendo SDKs are used in any part of it, as such there are no legal issues with it.

## Building

In order to build it, you will need to have Scons as well as Devkitpro's SDKs installed with the DevkitARM toolchain and libctru.

You will also need to have the following portlibs installed : 3ds-freetype, 3ds-bzip2, 3ds-libpng and 3ds-zlib.

Devkitpro's install instructions are available at : https://devkitpro.org/wiki/Getting_Started

Ensure that devkitpro/tools/bin is in your environment PATH so that 3dsxtool and picasso can be called by the build system (I do plan to make it instead rely on the devkitpro path defined to make it easier in the long run).

You can check if it is the case by typing `3dsxtool` in the command line, if you get an error that says it isn't found, you'll need to add the path to your PATH environment variable.

Once you have everything, you should be able to build godot 3DS using
`scons platform=3ds target=template_release devkitpro=/path/to/devkitpro`

# Godot Engine

<p align="center">
  <a href="https://godotengine.org">
    <img src="logo_outlined.svg" width="400" alt="Godot Engine logo">
  </a>
</p>

## 2D and 3D cross-platform game engine

**[Godot Engine](https://godotengine.org) is a feature-packed, cross-platform
game engine to create 2D and 3D games from a unified interface.** It provides a
comprehensive set of [common tools](https://godotengine.org/features), so that
users can focus on making games without having to reinvent the wheel. Games can
be exported with one click to a number of platforms, including the major desktop
platforms (Linux, macOS, Windows), mobile platforms (Android, iOS), as well as
Web-based platforms and [consoles](https://docs.godotengine.org/en/latest/tutorials/platform/consoles.html).

## Free, open source and community-driven

Godot is completely free and open source under the very permissive [MIT license](https://godotengine.org/license).
No strings attached, no royalties, nothing. The users' games are theirs, down
to the last line of engine code. Godot's development is fully independent and
community-driven, empowering users to help shape their engine to match their
expectations. It is supported by the [Godot Foundation](https://godot.foundation/)
not-for-profit.

Before being open sourced in [February 2014](https://github.com/godotengine/godot/commit/0b806ee0fc9097fa7bda7ac0109191c9c5e0a1ac),
Godot had been developed by [Juan Linietsky](https://github.com/reduz) and
[Ariel Manzur](https://github.com/punto-) (both still maintaining the project)
for several years as an in-house engine, used to publish several work-for-hire
titles.

![Screenshot of a 3D scene in the Godot Engine editor](https://raw.githubusercontent.com/godotengine/godot-design/master/screenshots/editor_tps_demo_1920x1080.jpg)

## Getting the engine

### Binary downloads

Official binaries for the Godot editor and the export templates can be found
[on the Godot website](https://godotengine.org/download).

### Compiling from source

[See the official docs](https://docs.godotengine.org/en/latest/contributing/development/compiling)
for compilation instructions for every supported platform.

## Community and contributing

Godot is not only an engine but an ever-growing community of users and engine
developers. The main community channels are listed [on the homepage](https://godotengine.org/community).

The best way to get in touch with the core engine developers is to join the
[Godot Contributors Chat](https://chat.godotengine.org).

To get started contributing to the project, see the [contributing guide](CONTRIBUTING.md).
This document also includes guidelines for reporting bugs.

## Documentation and demos

The official documentation is hosted on [Read the Docs](https://docs.godotengine.org).
It is maintained by the Godot community in its own [GitHub repository](https://github.com/godotengine/godot-docs).

The [class reference](https://docs.godotengine.org/en/latest/classes/)
is also accessible from the Godot editor.

We also maintain official demos in their own [GitHub repository](https://github.com/godotengine/godot-demo-projects)
as well as a list of [awesome Godot community resources](https://github.com/godotengine/awesome-godot).

There are also a number of other
[learning resources](https://docs.godotengine.org/en/latest/community/tutorials.html)
provided by the community, such as text and video tutorials, demos, etc.
Consult the [community channels](https://godotengine.org/community)
for more information.

[![Code Triagers Badge](https://www.codetriage.com/godotengine/godot/badges/users.svg)](https://www.codetriage.com/godotengine/godot)
[![Translate on Weblate](https://hosted.weblate.org/widgets/godot-engine/-/godot/svg-badge.svg)](https://hosted.weblate.org/engage/godot-engine/?utm_source=widget)
[![TODOs](https://badgen.net/https/api.tickgit.com/badgen/github.com/godotengine/godot)](https://www.tickgit.com/browse?repo=github.com/godotengine/godot)
