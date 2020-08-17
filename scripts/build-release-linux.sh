cd switch-tools
gcc -static -o build_romfs src/build_romfs.c src/romfs.c src/filepath.c
gcc -static -o nxlink src/nxlink.c -lz
cd ..

mv switch-tools/nxlink ./nxlink
mv switch-tools/build_romfs ./build_romfs

tar czf godot-linux.tar.gz godot.x11.opt.tools.64 build_romfs nxlink