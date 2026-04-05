# MyFin Player

MyFin is a Linux desktop music player built with C++ and Qt Quick/QML. The current base provides:

- local music library scanning
- playback queue with play, pause, stop, next, previous and seek
- volume, shuffle and repeat controls
- dynamic theming from album art colors
- Jellyfin authentication and remote audio listing
- persistent settings with `QSettings`
- CMake project structure prepared for Arch Linux packaging

## Architecture

The application is split into focused modules:

- `src/player`
  - `PlaybackController` owns `QMediaPlayer` and `QAudioOutput`, manages queue, playback state, shuffle and repeat.
- `src/library`
  - `LibraryManager` scans local music directories and merges remote Jellyfin tracks into a unified library model.
- `src/integration`
  - `JellyfinClient` authenticates against Jellyfin and fetches the user audio library through the HTTP API.
- `src/settings`
  - `SettingsManager` persists Jellyfin credentials, music folders and general player preferences.
- `src/theme`
  - `ThemeManager` extracts dominant and vibrant colors from cover art and exposes a readable palette to QML.
- `src/models`
  - `TrackListModel` is the shared Qt model used by library and queue views.
- `src/app`
  - `ApplicationController` wires all modules together and presents a clean bridge to QML.
- `qml`
  - desktop UI with pages for home, library, now playing, queue and settings

This layout keeps domain logic in C++ and the interaction layer in QML, which makes the project easier to extend later with richer metadata parsing, cover cache, MPRIS support, notifications and offline Jellyfin sync.

## Project structure

```text
.
├── CMakeLists.txt
├── README.md
├── packaging
│   ├── PKGBUILD
│   └── myfin.desktop
├── qml
│   ├── Main.qml
│   ├── components
│   └── pages
└── src
    ├── app
    ├── core
    ├── integration
    ├── library
    ├── models
    ├── player
    ├── settings
    └── theme
```

## Build on Arch Linux

Install dependencies:

```bash
sudo pacman -S --needed base-devel cmake qt6-base qt6-declarative qt6-multimedia
```

Configure and build:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(nproc)"
```

Run:

```bash
./build/myfin
```

## Packaging for Arch Linux

The repository includes a starter `packaging/PKGBUILD`.

Suggested flow:

```bash
cd packaging
makepkg -si
```

If you move this repository to a VCS source later, adjust `source=()` and `sha256sums=()` in the PKGBUILD accordingly.

## Jellyfin notes

- The initial implementation authenticates with `AuthenticateByName`.
- Remote library loading currently focuses on audio items only.
- Stream and cover URLs are generated from the server base URL and access token.
- Error states are surfaced in the settings and home screens.

## Next practical expansions

- metadata extraction for local files via TagLib
- cover cache on disk
- MPRIS integration
- drag and drop queue management
- search and filtering
- playlists and favorites
- richer Jellyfin sync and offline caching
