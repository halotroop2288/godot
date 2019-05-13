export PATH=$PATH:/opt/devkitpro/devkitA64/bin:/opt/devkitpro/portlibs/switch/bin:/opt/devkitpro/tools/bin

###
## create template export directory
mkdir -p ~/.local/share/godot/templates/3.1.1.stable/
###

###
## build switch release binary
###
scons platform=switch tools=no target=release module_etc_enabled=no module_webm_enabled=no module_websocket_enabled=no module_upnp_enabled=no $@ -j8
# copy binary to godot templates for switch export
cp bin/switch_release.nro ~/.local/share/godot/templates/3.1.1.stable/

###
## build x11 editor binary (x64)
###
scons platform=x11 tools=yes target=release_debug bits=64 module_etc_enabled=no module_webm_enabled=no module_websocket_enabled=no module_upnp_enabled=no $@ -j8
# copy binary to godot templates for x11 export
cp bin/godot.x11.opt.tools.64 ~/.local/share/godot/templates/3.1.1.stable/linux_x11_64_release
