import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import MyFin

ApplicationWindow {
    id: window

    width: 1560
    height: 960
    minimumWidth: 1180
    minimumHeight: 760
    visible: true
    title: "MyFin Player"
    color: "transparent"

    property int currentPage: 0

    Component.onCompleted: {
        const startup = AppController.settings.startupBehavior
        if (startup === "library")
            currentPage = 1
        else if (startup === "nowplaying")
            currentPage = 2
        else
            currentPage = 0
    }

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: Qt.lighter(AppController.theme.secondaryColor, 1.12) }
            GradientStop { position: 0.35; color: AppController.theme.primaryColor }
            GradientStop { position: 1.0; color: Qt.darker(AppController.theme.primaryColor, 1.18) }
        }

        Rectangle {
            anchors.fill: parent
            color: Qt.rgba(0.03, 0.05, 0.08, 0.35)
        }

        Rectangle {
            x: width * 0.55
            y: -80
            width: 520
            height: 520
            radius: 260
            color: Qt.rgba(AppController.theme.accentColor.r, AppController.theme.accentColor.g, AppController.theme.accentColor.b, 0.16)
            opacity: 0.9
        }

        Rectangle {
            x: -140
            y: height * 0.45
            width: 420
            height: 420
            radius: 210
            color: Qt.rgba(AppController.theme.secondaryColor.r, AppController.theme.secondaryColor.g, AppController.theme.secondaryColor.b, 0.14)
        }
    }

    Row {
        anchors.fill: parent
        anchors.margins: 22
        spacing: 18

        Rectangle {
            width: 230
            height: parent.height
            radius: 30
            color: Qt.rgba(0.05, 0.07, 0.10, 0.44)
            border.width: 1
            border.color: Qt.rgba(1, 1, 1, 0.08)

            Column {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 16

                Column {
                    spacing: 4

                    Text {
                        text: "MyFin"
                        color: AppController.theme.textColor
                        font.pixelSize: 30
                        font.bold: true
                    }

                    Text {
                        text: "Linux music player"
                        color: Qt.rgba(1, 1, 1, 0.60)
                        font.pixelSize: 13
                    }
                }

                SidebarButton {
                    width: parent.width
                    text: "Home"
                    active: window.currentPage === 0
                    accentColor: AppController.theme.accentColor
                    textColor: AppController.theme.textColor
                    onClicked: window.currentPage = 0
                }

                SidebarButton {
                    width: parent.width
                    text: "Musicas"
                    active: window.currentPage === 1
                    accentColor: AppController.theme.accentColor
                    textColor: AppController.theme.textColor
                    onClicked: window.currentPage = 1
                }

                SidebarButton {
                    width: parent.width
                    text: "Tocando Agora"
                    active: window.currentPage === 2
                    accentColor: AppController.theme.accentColor
                    textColor: AppController.theme.textColor
                    onClicked: window.currentPage = 2
                }

                SidebarButton {
                    width: parent.width
                    text: "Fila"
                    active: window.currentPage === 3
                    accentColor: AppController.theme.accentColor
                    textColor: AppController.theme.textColor
                    onClicked: window.currentPage = 3
                }

                SidebarButton {
                    width: parent.width
                    text: "Configuracoes"
                    active: window.currentPage === 4
                    accentColor: AppController.theme.accentColor
                    textColor: AppController.theme.textColor
                    onClicked: window.currentPage = 4
                }

                Item {
                    width: 1
                    height: 20
                }

                Rectangle {
                    width: parent.width
                    height: 150
                    radius: 24
                    color: Qt.rgba(1, 1, 1, 0.05)

                    Column {
                        anchors.fill: parent
                        anchors.margins: 16
                        spacing: 10

                        Text {
                            text: AppController.jellyfin.connected ? "Jellyfin online" : "Modo local"
                            color: AppController.theme.textColor
                            font.pixelSize: 16
                            font.bold: true
                        }

                        Text {
                            text: AppController.library.statusMessage
                            color: Qt.rgba(1, 1, 1, 0.68)
                            width: parent.width - 32
                            wrapMode: Text.WordWrap
                            font.pixelSize: 12
                        }
                    }
                }
            }
        }

        Item {
            width: parent.width - 248
            height: parent.height

            Loader {
                id: pageLoader
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: transportBar.top
                anchors.bottomMargin: 18
                sourceComponent: currentPage === 0 ? homePage :
                                 currentPage === 1 ? libraryPage :
                                 currentPage === 2 ? nowPlayingPage :
                                 currentPage === 3 ? queuePage : settingsPage
            }

            TransportBar {
                id: transportBar
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: 150
            }
        }
    }

    Component { id: homePage; HomePage {} }
    Component { id: libraryPage; LibraryPage {} }
    Component { id: nowPlayingPage; NowPlayingPage {} }
    Component { id: queuePage; QueuePage {} }
    Component { id: settingsPage; SettingsPage {} }
}
