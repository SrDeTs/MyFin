# MyFin

MyFin is a Linux-first Jellyfin music client built with C++20, Qt 6, and QML.

This repository currently contains the initial application skeleton:

- modular Qt/C++ project layout
- QML desktop shell with premium-oriented layout
- application services for settings, logging, playback state, Linux integration, and Jellyfin access points
- viewmodels and list models wired from C++ to QML
- packaging placeholders for Linux desktop integration

## Build

Prerequisites:

- CMake 3.25+
- Ninja or Make
- Qt 6.8+ with `Core`, `Gui`, `Qml`, `Quick`, `QuickControls2`, `Network`, `DBus`, `Concurrent`

Example:

```bash
cmake -S . -B build -G Ninja
cmake --build build
./build/myfin
```

## Current Scope

This is a clean skeleton intended to become a high-performance Jellyfin music player for Linux desktop. The current code focuses on:

- stable project structure
- low-friction expansion points
- separation between UI and logic
- Linux-first paths and services

## Next Steps

- replace demo Jellyfin data with real REST and websocket integration
- connect playback controller to a real Qt Multimedia backend
- add SQLite cache, incremental sync, image pipeline, and MPRIS implementation
