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
                    spacing: 6

                    Text {
                        text: "MYFIN"
                        color: theme.text
                        font.pixelSize: 29
                        font.letterSpacing: 2
                        font.weight: Font.Black
                    }

                    Text {
                        text: "Jellyfin music for Linux"
                        color: theme.textMuted
                        font.pixelSize: 13
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

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 74
                radius: theme.radiusLarge
                color: theme.panel
                border.width: 1
                border.color: theme.stroke

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 18
                    spacing: 16

                    Text {
                        text: window.currentSection === "musicas" ? "Musicas" : "Configuracoes"
                        color: theme.text
                        font.pixelSize: 24
                        font.weight: Font.Black
                    }

                    Text {
                        text: window.currentSection === "musicas"
                              ? AppVm.library.statusMessage
                              : AppVm.session.authenticated
                                ? AppVm.session.summary
                                : "Gerencie sua conexao com o Jellyfin"
                        color: theme.textMuted
                        font.pixelSize: 12
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }
                }
            }

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
