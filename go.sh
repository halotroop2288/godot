export PATH=$PATH:/opt/devkitpro/devkitA64/bin:/opt/devkitpro/portlibs/switch/bin:/opt/devkitpro/tools/bin
scons platform=switch tools=no target=release module_etc_enabled=no module_webm_enabled=no module_websocket_enabled=no module_upnp_enabled=no $@ -j8
# copy binary to godot templates for switch export
mkdir -p ~/.local/share/godot/templates/3.1.1.stable/
cp bin/switch_release.nro ~/.local/share/godot/templates/3.1.1.stable/
