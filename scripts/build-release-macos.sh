cd switch-tools
clang -o build_romfs src/build_romfs.c src/romfs.c src/filepath.c
clang -o nxlink src/nxlink.c -lz
cd ..

mkdir -p "godot_dmg/"
cp -r "misc/dist/osx_tools.app/" "godot_dmg/Godot.app/"
mkdir -p "godot_dmg/Godot.app/Contents/MacOS/"
cp "godot.osx.opt.tools.64" "godot_dmg/Godot.app/Contents/MacOS/Godot"
cp switch-tools/nxlink switch-tools/build_romfs "godot_dmg/Godot.app/Contents/MacOS/"

git clone "https://github.com/andreyvit/create-dmg.git" --depth=1
(
  cd "create-dmg/"
  ./create-dmg \
  	  --skip-jenkins \
      --volname "Godot" \
      --volicon "../godot_dmg/Godot.app/Contents/Resources/Godot.icns" \
      --hide-extension "Godot.app" \
      "../godot-macos-x86_64.dmg" \
      "../godot_dmg/"
)