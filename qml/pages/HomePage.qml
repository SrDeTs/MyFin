import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import MyFin

Item {
    id: root

    property var app: AppController
    property color textColor: AppController.theme.textColor

    Column {
        anchors.fill: parent
        spacing: 22

        Rectangle {
            width: parent.width
            height: 270
            radius: 34
            color: Qt.rgba(1, 1, 1, 0.08)
            border.width: 1
            border.color: Qt.rgba(1, 1, 1, 0.08)

            Row {
                anchors.fill: parent
                anchors.margins: 26
                spacing: 24

                CoverArt {
                    width: 210
                    height: 210
                    anchors.verticalCenter: parent.verticalCenter
                    source: AppController.playback.currentTrack.artUrl
                    accentColor: AppController.theme.accentColor
                    fallbackTitle: AppController.playback.currentTrack.title ? AppController.playback.currentTrack.title[0] : "M"
                }

                Column {
                    width: parent.width - 260
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 12

                    Text {
                        text: "MyFin"
                        color: Qt.rgba(1, 1, 1, 0.68)
                        font.pixelSize: 14
                        font.letterSpacing: 2
                    }

                    Text {
                        text: AppController.playback.currentTrack.title || "Seu player de musica para Linux"
                        color: root.textColor
                        font.pixelSize: 36
                        font.bold: true
                        wrapMode: Text.WordWrap
                        width: parent.width
                    }

                    Text {
                        text: AppController.playback.currentTrack.artist
                              ? AppController.playback.currentTrack.artist + "  |  " + (AppController.playback.currentTrack.album || "Single")
                              : "Biblioteca local e Jellyfin, com tema dinamico por capa e interface feita para desktop Linux."
                        color: Qt.rgba(1, 1, 1, 0.78)
                        font.pixelSize: 16
                        wrapMode: Text.WordWrap
                        width: parent.width
                    }

                    Row {
                        spacing: 12

                        AppButton {
                            width: 120
                            height: 46
                            text: AppController.playback.playing ? "Pause" : "Play"
                            accentColor: AppController.theme.accentColor
                            textColor: root.textColor
                            onClicked: AppController.playback.playing ? AppController.playback.pause() : AppController.playback.play()
                        }

                        AppButton {
                            width: 150
                            height: 46
                            text: "Atualizar local"
                            subtle: true
                            accentColor: AppController.theme.accentColor
                            textColor: root.textColor
                            onClicked: AppController.reloadLocalLibrary()
                        }

                        AppButton {
                            width: 156
                            height: 46
                            text: AppController.jellyfin.connected ? "Sync Jellyfin" : "Conectar Jellyfin"
                            subtle: true
                            accentColor: AppController.theme.accentColor
                            textColor: root.textColor
                            onClicked: AppController.connectJellyfin()
                        }
                    }
                }
            }
        }

        RowLayout {
            width: parent.width
            spacing: 18

            Rectangle {
                Layout.preferredWidth: parent.width * 0.38
                Layout.fillHeight: true
                height: 240
                radius: 28
                color: Qt.rgba(1, 1, 1, 0.06)

                Column {
                    anchors.fill: parent
                    anchors.margins: 22
                    spacing: 14

                    Text {
                        text: "Status"
                        color: root.textColor
                        font.pixelSize: 20
                        font.bold: true
                    }

                    Text {
                        text: AppController.library.statusMessage
                        color: Qt.rgba(1, 1, 1, 0.72)
                        width: parent.width - 44
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        text: AppController.jellyfin.errorString.length > 0
                              ? AppController.jellyfin.errorString
                              : (AppController.jellyfin.connected ? "Jellyfin conectado." : "Jellyfin ainda nao conectado.")
                        color: AppController.jellyfin.errorString.length > 0 ? "#ffd4d4" : Qt.rgba(1, 1, 1, 0.72)
                        width: parent.width - 44
                        wrapMode: Text.WordWrap
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                height: 240
                radius: 28
                color: Qt.rgba(1, 1, 1, 0.06)

                Column {
                    anchors.fill: parent
                    anchors.margins: 22
                    spacing: 14

                    Text {
                        text: "Fila atual"
                        color: root.textColor
                        font.pixelSize: 20
                        font.bold: true
                    }

                    ListView {
                        width: parent.width
                        height: 170
                        model: AppController.playback.queueModel
                        spacing: 8

                        delegate: Rectangle {
                            width: ListView.view.width
                            height: 48
                            radius: 16
                            color: index === AppController.playback.currentIndex
                                   ? Qt.rgba(AppController.theme.accentColor.r, AppController.theme.accentColor.g, AppController.theme.accentColor.b, 0.20)
                                   : Qt.rgba(1, 1, 1, 0.04)

                            Row {
                                anchors.fill: parent
                                anchors.margins: 12
                                spacing: 12

                                Text {
                                    text: model.title
                                    color: root.textColor
                                    font.pixelSize: 14
                                    elide: Text.ElideRight
                                    width: parent.width - 24
                                }

                                Text {
                                    text: model.artist
                                    color: Qt.rgba(1, 1, 1, 0.66)
                                    font.pixelSize: 12
                                }
                            }
                        }

                        Text {
                            anchors.centerIn: parent
                            visible: AppController.playback.queueModel.count === 0
                            text: "Sua fila de reproducao aparecera aqui."
                            color: Qt.rgba(1, 1, 1, 0.56)
                        }
                    }
                }
            }
        }
    }
}
