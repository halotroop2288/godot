export PATH=$PATH:/opt/devkitpro/devkitA64/bin:/opt/devkitpro/portlibs/switch/bin:/opt/devkitpro/toosl/bin
scons platform=switch module_etc_enabled=no module_webm_enabled=no module_websocket_enabled=no module_upnp_enabled=no $@ -j4
