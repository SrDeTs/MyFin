import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import MyFin

Item {
    RowLayout {
        anchors.fill: parent
        spacing: 24

        Rectangle {
            Layout.preferredWidth: parent.width * 0.42
            Layout.fillHeight: true
            radius: 30
            color: Qt.rgba(1, 1, 1, 0.06)

            Column {
                anchors.fill: parent
                anchors.margins: 28
                spacing: 18

                CoverArt {
                    width: parent.width - 56
                    height: width
                    source: AppController.playback.currentTrack.artUrl
                    accentColor: AppController.theme.accentColor
                    fallbackTitle: AppController.playback.currentTrack.title ? AppController.playback.currentTrack.title[0] : "M"
                }

                Text {
                    text: AppController.playback.currentTrack.title || "Nada tocando"
                    color: AppController.theme.textColor
                    font.pixelSize: 32
                    font.bold: true
                    width: parent.width
                    wrapMode: Text.WordWrap
                }

                Text {
                    text: AppController.playback.currentTrack.artist || "Escolha uma musica na biblioteca"
                    color: Qt.rgba(1, 1, 1, 0.74)
                    font.pixelSize: 18
                }

                Text {
                    text: AppController.playback.currentTrack.album || AppController.playback.currentTrack.sourceName || ""
                    color: Qt.rgba(1, 1, 1, 0.58)
                    font.pixelSize: 14
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: 30
            color: Qt.rgba(1, 1, 1, 0.06)

            Column {
                anchors.fill: parent
                anchors.margins: 28
                spacing: 18

                Text {
                    text: "Controles e ambiente"
                    color: AppController.theme.textColor
                    font.pixelSize: 24
                    font.bold: true
                }

                Rectangle {
                    width: parent.width
                    height: 140
                    radius: 24
                    color: Qt.rgba(1, 1, 1, 0.04)

                    Column {
                        anchors.fill: parent
                        anchors.margins: 20
                        spacing: 14

                        Text {
                            text: "Tema dinamico baseado na arte atual."
                            color: AppController.theme.textColor
                            font.pixelSize: 16
                        }

                        Row {
                            spacing: 12

                            Repeater {
                                model: [AppController.theme.primaryColor, AppController.theme.secondaryColor, AppController.theme.accentColor]

                                Rectangle {
                                    width: 90
                                    height: 60
                                    radius: 18
                                    color: modelData
                                    border.width: 1
                                    border.color: Qt.rgba(1, 1, 1, 0.16)
                                }
                            }
                        }
                    }
                }

                Rectangle {
                    width: parent.width
                    height: parent.height - 222
                    radius: 24
                    color: Qt.rgba(1, 1, 1, 0.04)

                    Column {
                        anchors.fill: parent
                        anchors.margins: 20
                        spacing: 12

                        Text {
                            text: "Proximas faixas"
                            color: AppController.theme.textColor
                            font.pixelSize: 18
                            font.bold: true
                        }

                        ListView {
                            width: parent.width
                            height: parent.height - 40
                            model: AppController.playback.queueModel
                            spacing: 8

                            delegate: Rectangle {
                                width: ListView.view.width
                                height: 56
                                radius: 18
                                color: index === AppController.playback.currentIndex
                                       ? Qt.rgba(AppController.theme.accentColor.r, AppController.theme.accentColor.g, AppController.theme.accentColor.b, 0.18)
                                       : Qt.rgba(1, 1, 1, 0.04)

                                Row {
                                    anchors.fill: parent
                                    anchors.margins: 12
                                    spacing: 12

                                    Text {
                                        text: model.title
                                        color: AppController.theme.textColor
                                        font.pixelSize: 14
                                        width: parent.width - 140
                                        elide: Text.ElideRight
                                    }

                                    AppButton {
                                        width: 56
                                        height: 34
                                        text: "Play"
                                        accentColor: AppController.theme.accentColor
                                        textColor: AppController.theme.textColor
                                        onClicked: AppController.playFromQueue(index)
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
