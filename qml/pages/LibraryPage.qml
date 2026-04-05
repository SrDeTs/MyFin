import QtQuick
import QtQuick.Controls
import MyFin

Item {
    Column {
        anchors.fill: parent
        spacing: 18

        Row {
            width: parent.width
            spacing: 12

            Text {
                text: "Biblioteca"
                color: AppController.theme.textColor
                font.pixelSize: 28
                font.bold: true
            }

            Rectangle {
                width: 1
                height: 26
                color: Qt.rgba(1, 1, 1, 0.12)
                anchors.verticalCenter: parent.verticalCenter
            }

            Text {
                text: AppController.library.tracksModel.count + " faixas"
                color: Qt.rgba(1, 1, 1, 0.64)
                font.pixelSize: 14
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        ListView {
            width: parent.width
            height: parent.height - 50
            model: AppController.library.tracksModel
            spacing: 10
            clip: true

            delegate: TrackDelegate {
                width: ListView.view.width
                title: model.title
                artist: model.artist
                album: model.album
                artUrl: model.artUrl
                durationMs: model.durationMs
                sourceName: model.sourceName
                active: index === AppController.playback.currentIndex
                accentColor: AppController.theme.accentColor
                textColor: AppController.theme.textColor
                onPlayRequested: AppController.playLibraryTrack(index)
                onEnqueueRequested: AppController.enqueueLibraryTrack(index)
            }

            Text {
                anchors.centerIn: parent
                visible: AppController.library.tracksModel.count === 0
                text: AppController.library.statusMessage
                color: Qt.rgba(1, 1, 1, 0.56)
                width: parent.width * 0.5
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
}
