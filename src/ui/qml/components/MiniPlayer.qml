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

    implicitHeight: 112

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 10

        RowLayout {
            Layout.fillWidth: true
            spacing: 14

            Rectangle {
                Layout.preferredWidth: 60
                Layout.preferredHeight: 60
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
                    sourceSize.width: 60
                    sourceSize.height: 60
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
                    text: playback.hasTrack ? playback.title : "Nada tocando"
                    color: theme.text
                    font.pixelSize: 17
                    font.weight: Font.DemiBold
                    elide: Text.ElideRight
                }

                Text {
                    text: playback.hasTrack
                          ? playback.artist + " • " + playback.album
                          : "Selecione uma faixa para iniciar a reprodução"
                    color: theme.textMuted
                    font.pixelSize: 13
                    elide: Text.ElideRight
                }
            }

            RowLayout {
                spacing: 10

                ToolButton {
                    text: playback.playing ? "||" : ">"
                    enabled: playback.hasTrack
                    onClicked: playback.togglePlaying()

                    font.pixelSize: 18
                    font.weight: Font.Black
                    leftPadding: 0
                    rightPadding: 0
                    topPadding: 0
                    bottomPadding: 0

                    background: Rectangle {
                        implicitWidth: 48
                        implicitHeight: 48
                        radius: 24
                        color: parent.enabled ? theme.accentStrong : theme.panelRaised
                        border.width: 1
                        border.color: theme.stroke
                    }

                    contentItem: Text {
                        text: parent.text
                        color: parent.enabled ? theme.window : theme.textMuted
                        font.pixelSize: parent.font.pixelSize
                        font.weight: parent.font.weight
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                ToolButton {
                    text: ">>"
                    enabled: playback.canGoNext
                    onClicked: playback.next()

                    font.pixelSize: 15
                    font.weight: Font.Black
                    leftPadding: 0
                    rightPadding: 0
                    topPadding: 0
                    bottomPadding: 0

                    background: Rectangle {
                        implicitWidth: 42
                        implicitHeight: 42
                        radius: 21
                        color: parent.enabled ? theme.panelRaised : theme.panel
                        border.width: 1
                        border.color: theme.stroke
                    }

                    contentItem: Text {
                        text: parent.text
                        color: parent.enabled ? theme.text : theme.textMuted
                        font.pixelSize: parent.font.pixelSize
                        font.weight: parent.font.weight
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            Text {
                text: playback.positionLabel
                color: theme.textMuted
                font.pixelSize: 12
                Layout.preferredWidth: 42
                horizontalAlignment: Text.AlignRight
            }

            Slider {
                id: progressSlider
                Layout.fillWidth: true
                enabled: playback.hasTrack && playback.durationMs > 0
                from: 0
                to: Math.max(playback.durationMs, 1)
                value: pressed ? value : playback.positionMs
                onMoved: playback.seek(Math.round(value))

                background: Rectangle {
                    x: progressSlider.leftPadding
                    y: progressSlider.topPadding + progressSlider.availableHeight / 2 - height / 2
                    width: progressSlider.availableWidth
                    height: 4
                    radius: 2
                    color: theme.panelRaised

                    Rectangle {
                        width: progressSlider.visualPosition * parent.width
                        height: parent.height
                        radius: parent.radius
                        color: theme.accentStrong
                    }
                }

                handle: Rectangle {
                    x: progressSlider.leftPadding + progressSlider.visualPosition * (progressSlider.availableWidth - width)
                    y: progressSlider.topPadding + progressSlider.availableHeight / 2 - height / 2
                    width: 14
                    height: 14
                    radius: 7
                    color: theme.text
                    border.width: 1
                    border.color: theme.stroke
                }
            }

            Text {
                text: playback.durationLabel
                color: theme.textMuted
                font.pixelSize: 12
                Layout.preferredWidth: 42
            }
        }
    }
}
