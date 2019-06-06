export PATH=$PATH:/opt/devkitpro/devkitA64/bin:/opt/devkitpro/portlibs/switch/bin:/opt/devkitpro/tools/bin
scons platform=switch tools=no target=debug module_websocket_enabled=no module_upnp_enabled=no $@ -j8
