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
            Layout.preferredWidth: 198
            Layout.fillHeight: true
            radius: theme.radiusLarge
            color: theme.panel
            border.width: 1
            border.color: theme.stroke

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 18
                spacing: 18

                ColumnLayout {
                    spacing: 0

                    Text {
                        text: "MYFIN"
                        color: theme.text
                        font.pixelSize: 29
                        font.letterSpacing: 2
                        font.weight: Font.Black
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    SidebarButton {
                        text: "Musicas"
                        active: window.currentSection === "musicas"
                        onClicked: window.currentSection = "musicas"
                    }

                    SidebarButton {
                        text: "Configuracoes"
                        active: window.currentSection === "configuracoes"
                        onClicked: window.currentSection = "configuracoes"
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

            SettingsScreen {
                visible: window.currentSection === "configuracoes"
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
