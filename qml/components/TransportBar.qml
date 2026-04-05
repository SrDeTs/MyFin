import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import MyFin

Rectangle {
    id: root

    property var playback: AppController.playback
    property var theme: AppController.theme
    property color textColor: theme.textColor

    radius: 26
    color: Qt.rgba(0.05, 0.07, 0.10, 0.52)
    border.width: 1
    border.color: Qt.rgba(1, 1, 1, 0.08)

    readonly property real infoPanelWidth: Math.min(360, root.width * 0.34)
    readonly property real statusPanelWidth: Math.min(320, root.width * 0.28)
    readonly property real controlsMinWidth: 210

    Column {
        anchors.fill: parent
        anchors.margins: 18
        spacing: 12

        Slider {
            id: positionSlider
            from: 0
            to: Math.max(1, playback.duration)
            value: playback.position
            onMoved: playback.seek(value)

            background: Rectangle {
                x: positionSlider.leftPadding
                y: positionSlider.topPadding + positionSlider.availableHeight / 2 - height / 2
                width: positionSlider.availableWidth
                height: 6
                radius: 3
                color: Qt.rgba(1, 1, 1, 0.12)

                Rectangle {
                    width: positionSlider.visualPosition * parent.width
                    height: parent.height
                    radius: 3
                    color: root.theme.accentColor
                }
            }

            handle: Rectangle {
                x: positionSlider.leftPadding + positionSlider.visualPosition * (positionSlider.availableWidth - width)
                y: positionSlider.topPadding + positionSlider.availableHeight / 2 - height / 2
                width: 16
                height: 16
                radius: 8
                color: root.theme.accentColor
            }
        }

        Item {
            width: parent.width
            height: 84

            Row {
                id: infoRow
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                width: root.infoPanelWidth
                spacing: 14

                CoverArt {
                    width: 64
                    height: 64
                    anchors.verticalCenter: parent.verticalCenter
                    source: playback.currentTrack.artUrl
                    accentColor: root.theme.accentColor
                    fallbackTitle: playback.currentTrack.title ? playback.currentTrack.title[0] : "M"
                }

                Column {
                    anchors.verticalCenter: parent.verticalCenter
                    width: infoRow.width - 78
                    spacing: 4

                    Text {
                        text: playback.currentTrack.title || "Nada tocando"
                        color: root.textColor
                        font.pixelSize: 17
                        font.bold: true
                        elide: Text.ElideRight
                        width: parent.width
                    }

                    Text {
                        text: (playback.currentTrack.artist || "Sua biblioteca") + "  |  " + (playback.currentTrack.album || playback.currentTrack.sourceName || "Player")
                        color: Qt.rgba(1, 1, 1, 0.72)
                        font.pixelSize: 13
                        elide: Text.ElideRight
                        width: parent.width
                    }
                }
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                spacing: 10

                AppButton {
                    width: 56
                    height: 40
                    text: "Prev"
                    subtle: true
                    accentColor: root.theme.accentColor
                    textColor: root.textColor
                    onClicked: playback.previous()
                }

                AppButton {
                    width: 88
                    height: 44
                    text: playback.playing ? "Pause" : "Play"
                    accentColor: root.theme.accentColor
                    textColor: root.textColor
                    onClicked: playback.playing ? playback.pause() : playback.play()
                }

                AppButton {
                    width: 56
                    height: 40
                    text: "Next"
                    subtle: true
                    accentColor: root.theme.accentColor
                    textColor: root.textColor
                    onClicked: playback.next()
                }
            }

            Column {
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                width: root.statusPanelWidth
                spacing: 8

                Text {
                    text: AppController.formatTime(playback.position) + " / " + AppController.formatTime(playback.duration) + "   " + (playback.shuffle ? "Shuffle On" : "Shuffle Off")
                    color: Qt.rgba(1, 1, 1, 0.68)
                    font.pixelSize: 12
                    horizontalAlignment: Text.AlignRight
                    width: parent.width
                    elide: Text.ElideRight
                }

                RowLayout {
                    width: parent.width
                    spacing: 10

                    AppButton {
                        Layout.preferredWidth: 82
                        Layout.preferredHeight: 38
                        text: playback.shuffle ? "Shuffle" : "Linear"
                        subtle: true
                        accentColor: root.theme.accentColor
                        textColor: root.textColor
                        onClicked: playback.shuffle = !playback.shuffle
                    }

                    AppButton {
                        Layout.preferredWidth: 88
                        Layout.preferredHeight: 38
                        text: playback.repeatMode === 1 ? "Track" : (playback.repeatMode === 2 ? "Queue" : "Repeat Off")
                        subtle: true
                        accentColor: root.theme.accentColor
                        textColor: root.textColor
                        onClicked: playback.repeatMode = (playback.repeatMode + 1) % 3
                    }

                    Slider {
                        Layout.fillWidth: true
                        from: 0
                        to: 100
                        value: playback.volume
                        onMoved: playback.volume = value
                    }
                }
            }
        }
    }
}
