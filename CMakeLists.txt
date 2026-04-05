cmake_minimum_required(VERSION 3.21)

project(MyFinPlayer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS
    Core
    Gui
    Quick
    Qml
    Multimedia
    Network
)

qt_standard_project_setup(REQUIRES 6.5)
qt_policy(SET QTP0004 NEW)

qt_add_executable(myfin
    src/main.cpp
    src/core/track.cpp
    src/models/tracklistmodel.cpp
    src/settings/settingsmanager.cpp
    src/integration/jellyfinclient.cpp
    src/library/librarymanager.cpp
    src/player/playbackcontroller.cpp
    src/theme/thememanager.cpp
    src/app/applicationcontroller.cpp
)

target_include_directories(myfin PRIVATE src)

qt_add_qml_module(myfin
    URI MyFin
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/components/AppButton.qml
        qml/components/SidebarButton.qml
        qml/components/TrackDelegate.qml
        qml/components/TransportBar.qml
        qml/components/CoverArt.qml
        qml/components/AppTextField.qml
        qml/components/AppComboBox.qml
        qml/components/AppSwitch.qml
        qml/pages/HomePage.qml
        qml/pages/LibraryPage.qml
        qml/pages/NowPlayingPage.qml
        qml/pages/QueuePage.qml
        qml/pages/SettingsPage.qml
)

target_link_libraries(myfin PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Qml
    Qt6::Multimedia
    Qt6::Network
)

install(TARGETS myfin
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)

install(DIRECTORY packaging/
    DESTINATION share/myfin
)
