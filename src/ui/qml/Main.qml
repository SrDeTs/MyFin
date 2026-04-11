import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: window
    property string currentSection: "musicas"

    visible: true
    width: 1480
    height: 920
    minimumWidth: 1180
    minimumHeight: 760
    title: AppVm.windowTitle

    Theme {
        id: theme
    }

    color: theme.window

    RowLayout {
        anchors.fill: parent
        anchors.margins: 18
        spacing: 18

        Rectangle {
            Layout.preferredWidth: 172
            Layout.fillHeight: true
            radius: theme.radiusLarge
            color: theme.panel
            border.width: 1
            border.color: theme.stroke

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 14
                spacing: 16

                Rectangle {
                    Layout.fillWidth: true
                    radius: 18
                    color: theme.panelMuted
                    border.width: 1
                    border.color: theme.stroke
                    implicitHeight: 72

                    Text {
                        anchors.left: parent.left
                        anchors.leftMargin: 16
                        anchors.verticalCenter: parent.verticalCenter
                        text: "MYFIN"
                        color: theme.text
                        font.pixelSize: 23
                        font.letterSpacing: 1.4
                        font.weight: Font.Black
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 6

                    SidebarButton {
                        Layout.fillWidth: true
                        theme: theme
                        text: "Musicas"
                        active: window.currentSection === "musicas"
                        onClicked: window.currentSection = "musicas"
                    }

                    SidebarButton {
                        Layout.fillWidth: true
                        theme: theme
                        text: "Login Jellyfin"
                        active: window.currentSection === "login"
                        onClicked: window.currentSection = "login"
                    }

                    SidebarButton {
                        Layout.fillWidth: true
                        theme: theme
                        text: "Configuracoes"
                        active: window.currentSection === "config"
                        onClicked: window.currentSection = "config"
                    }
                }

                Item { Layout.fillHeight: true }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 18

            HomeScreen {
                visible: window.currentSection === "musicas"
                Layout.fillWidth: true
                Layout.fillHeight: true
                appVm: AppVm
                theme: theme
            }

            LoginScreen {
                visible: window.currentSection === "login"
                Layout.fillWidth: true
                Layout.fillHeight: true
                appVm: AppVm
                theme: theme
            }

            ConfigScreen {
                visible: window.currentSection === "config"
                Layout.fillWidth: true
                Layout.fillHeight: true
                appVm: AppVm
                theme: theme
            }

            MiniPlayer {
                visible: window.currentSection === "musicas"
                Layout.fillWidth: true
                playback: AppVm.playback
                theme: theme
            }
        }
    }
}
