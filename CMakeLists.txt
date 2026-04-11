cmake_minimum_required(VERSION 3.25)

project(MyFin
    VERSION 0.1.0
    DESCRIPTION "Linux-first Jellyfin music player"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 6.8 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    QuickControls2
    Network
    DBus
    Concurrent
    Multimedia
    Sql
)

qt_standard_project_setup(REQUIRES 6.8)

set(MYFIN_SOURCES
    src/app/main.cpp
    src/app/AppBootstrap.h
    src/app/AppBootstrap.cpp
    src/app/AppServices.h
    src/app/AppServices.cpp
    src/domain/entities/Track.h
    src/domain/entities/Album.h
    src/domain/entities/Artist.h
    src/domain/entities/Playlist.h
    src/domain/value_objects/ServerProfile.h
    src/infrastructure/logging/Logging.h
    src/infrastructure/logging/Logging.cpp
    src/infrastructure/settings/AppPaths.h
    src/infrastructure/settings/AppPaths.cpp
    src/infrastructure/settings/SettingsService.h
    src/infrastructure/settings/SettingsService.cpp
    src/infrastructure/storage/StateStore.h
    src/infrastructure/storage/StateStore.cpp
    src/infrastructure/storage/LibraryCache.h
    src/infrastructure/storage/LibraryCache.cpp
    src/infrastructure/images/CoverArtImageProvider.h
    src/infrastructure/images/CoverArtImageProvider.cpp
    src/infrastructure/jellyfin/JellyfinApiClient.h
    src/infrastructure/jellyfin/JellyfinApiClient.cpp
    src/infrastructure/playback/IPlaybackBackend.h
    src/infrastructure/playback/QtMediaPlaybackBackend.h
    src/infrastructure/playback/QtMediaPlaybackBackend.cpp
    src/infrastructure/playback/PlaybackController.h
    src/infrastructure/playback/PlaybackController.cpp
    src/linux/MprisService.h
    src/linux/MprisService.cpp
    src/linux/NotificationService.h
    src/linux/NotificationService.cpp
    src/linux/SecretStore.h
    src/linux/SecretStore.cpp
    src/linux/SingleInstanceService.h
    src/linux/SingleInstanceService.cpp
    src/presentation/models/TrackListModel.h
    src/presentation/models/TrackListModel.cpp
    src/presentation/viewmodels/LibraryViewModel.h
    src/presentation/viewmodels/LibraryViewModel.cpp
    src/presentation/viewmodels/AudioSettingsViewModel.h
    src/presentation/viewmodels/AudioSettingsViewModel.cpp
    src/presentation/viewmodels/PlaybackViewModel.h
    src/presentation/viewmodels/PlaybackViewModel.cpp
    src/presentation/viewmodels/SessionViewModel.h
    src/presentation/viewmodels/SessionViewModel.cpp
    src/presentation/viewmodels/AppRootViewModel.h
    src/presentation/viewmodels/AppRootViewModel.cpp
)

qt_add_executable(myfin
    ${MYFIN_SOURCES}
)

target_include_directories(myfin PRIVATE src)

target_link_libraries(myfin PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Network
    Qt6::DBus
    Qt6::Concurrent
    Qt6::Multimedia
    Qt6::Sql
)

qt_add_qml_module(myfin
    URI MyFin
    VERSION 1.0
    QML_FILES
        src/ui/qml/Main.qml
        src/ui/qml/theme/Theme.qml
        src/ui/qml/components/SidebarButton.qml
        src/ui/qml/components/AppButton.qml
        src/ui/qml/components/AppTextField.qml
        src/ui/qml/components/MiniPlayer.qml
        src/ui/qml/screens/HomeScreen.qml
        src/ui/qml/screens/LoginScreen.qml
        src/ui/qml/screens/ConfigScreen.qml
)

set_target_properties(myfin PROPERTIES
    OUTPUT_NAME myfin
)

install(TARGETS myfin
    RUNTIME DESTINATION bin
)

install(FILES
    dist/linux/myfin.desktop
    DESTINATION share/applications
)

install(FILES
    dist/linux/io.github.myfin.MyFin.metainfo.xml
    DESTINATION share/metainfo
)
