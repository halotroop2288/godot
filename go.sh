export PATH=$PATH:/opt/devkitpro/devkitA64/bin:/opt/devkitpro/portlibs/switch/bin:/opt/devkitpro/tools/bin
scons platform=switch tools=no module_etc_enabled=no module_webm_enabled=no module_websocket_enabled=no module_upnp_enabled=no $@ -j8
