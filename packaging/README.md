# Packaging

## DEB / RPM

Use CPack from the build directory:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
cd build
cpack -G DEB
cpack -G RPM
```

## AppImage

Prepare an AppDir:

```bash
./packaging/appimage/build-appimage.sh
```

Then generate the AppImage with `linuxdeploy` plus the Qt plugin.

## Arch / pacman

Use the provided `packaging/arch/PKGBUILD` as a starting point.

## Flatpak

Build with:

```bash
flatpak-builder build-flatpak packaging/flatpak/io.github.srdets.ResolveMediaConverter.yml --force-clean
```
