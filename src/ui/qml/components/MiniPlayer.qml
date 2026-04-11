import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    required property var playback
    required property var theme

    color: theme.panel
    radius: theme.radiusMedium
    border.width: 1
    border.color: theme.stroke

    implicitHeight: 86

    RowLayout {
        anchors.fill: parent
        anchors.margins: 18
        spacing: 16

        Rectangle {
            Layout.preferredWidth: 56
            Layout.preferredHeight: 56
            radius: 14
            color: theme.panelRaised
            border.width: 1
            border.color: theme.stroke

            Image {
                anchors.fill: parent
                anchors.margins: 1
                asynchronous: true
                cache: false
                fillMode: Image.PreserveAspectCrop
                sourceSize.width: 56
                sourceSize.height: 56
                source: playback.hasTrack ? playback.coverSource : ""
                visible: playback.hasTrack && playback.coverSource.length > 0
            }

            Text {
                anchors.centerIn: parent
                visible: !playback.hasTrack || playback.coverSource.length === 0
                text: "MF"
                color: theme.textMuted
                font.pixelSize: 14
                font.weight: Font.Bold
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2

            Text {
                text: playback.hasTrack ? playback.title : "Fila pronta"
                color: theme.text
                font.pixelSize: 16
                font.weight: Font.DemiBold
                elide: Text.ElideRight
            }

            Text {
                text: playback.hasTrack ? playback.artist + " • " + playback.album : "Selecione uma faixa da biblioteca"
                color: theme.textMuted
                font.pixelSize: 13
                elide: Text.ElideRight
            }
        }

        RowLayout {
            spacing: 10

            Button {
                text: playback.playing ? "Pause" : "Play"
                enabled: playback.hasTrack
                onClicked: playback.togglePlaying()
            }

            Button {
                text: "Next"
                enabled: playback.queueLength > 1
                onClicked: playback.next()
            }
        }
    }
}
