import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import MyFin

Item {
    Column {
        anchors.fill: parent
        spacing: 18

        Text {
            text: "Fila de reproducao"
            color: AppController.theme.textColor
            font.pixelSize: 28
            font.bold: true
        }

        ListView {
            width: parent.width
            height: parent.height - 46
            model: AppController.playback.queueModel
            spacing: 10

            delegate: Rectangle {
                width: ListView.view.width
                height: 76
                radius: 22
                color: index === AppController.playback.currentIndex
                       ? Qt.rgba(AppController.theme.accentColor.r, AppController.theme.accentColor.g, AppController.theme.accentColor.b, 0.18)
                       : Qt.rgba(1, 1, 1, 0.05)

                Row {
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 14

                    CoverArt {
                        width: 48
                        height: 48
                        anchors.verticalCenter: parent.verticalCenter
                        source: model.artUrl
                        accentColor: AppController.theme.accentColor
                        fallbackTitle: model.title ? model.title[0] : "M"
                    }

                    Column {
                        width: parent.width - 200
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 4

                        Text {
                            text: model.title
                            color: AppController.theme.textColor
                            font.pixelSize: 15
                            font.bold: true
                            elide: Text.ElideRight
                            width: parent.width
                        }

                        Text {
                            text: model.artist + "  |  " + model.sourceName
                            color: Qt.rgba(1, 1, 1, 0.66)
                            font.pixelSize: 12
                            width: parent.width
                            elide: Text.ElideRight
                        }
                    }

                    AppButton {
                        width: 56
                        height: 38
                        anchors.verticalCenter: parent.verticalCenter
                        text: "Play"
                        accentColor: AppController.theme.accentColor
                        textColor: AppController.theme.textColor
                        onClicked: AppController.playFromQueue(index)
                    }

                    AppButton {
                        width: 56
                        height: 38
                        anchors.verticalCenter: parent.verticalCenter
                        text: "Del"
                        subtle: true
                        accentColor: AppController.theme.accentColor
                        textColor: AppController.theme.textColor
                        onClicked: AppController.removeQueueTrack(index)
                    }
                }
            }

            Text {
                anchors.centerIn: parent
                visible: AppController.playback.queueModel.count === 0
                text: "Nenhuma faixa adicionada a fila."
                color: Qt.rgba(1, 1, 1, 0.56)
            }
        }
    }
}
