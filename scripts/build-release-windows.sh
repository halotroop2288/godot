sudo apt-get install mingw-w64 libz-mingw-w64-dev

cd switch-tools
x86_64-w64-mingw32-gcc -static -o build_romfs.exe src/build_romfs.c src/romfs.c src/filepath.c
x86_64-w64-mingw32-gcc -static -o nxlink.exe src/nxlink.c -lz -lws2_32
cd ..
mv switch-tools/nxlink.exe ./nxlink.exe
mv switch-tools/build_romfs.exe ./build_romfs.exe

zip godot-windows.zip godot.windows.opt.tools.64.exe build_romfs.exe nxlink.exe