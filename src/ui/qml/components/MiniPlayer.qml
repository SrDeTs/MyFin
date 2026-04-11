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

    implicitHeight: 128
    readonly property int volumeClusterWidth: 156

    function controlBgColor(enabled, highlighted, hovered) {
        if (!enabled) {
            return theme.panelMuted
        }
        if (highlighted) {
            return hovered ? theme.warm : theme.accentStrong
        }
        return hovered ? theme.panel : theme.panelRaised
    }

    function controlFgColor(enabled, highlighted) {
        if (!enabled) {
            return theme.textMuted
        }
        return highlighted ? theme.window : theme.text
    }

    function volumeIcon(volume) {
        if (volume <= 0.001) {
            return "\uD83D\uDD07"
        }
        if (volume < 0.45) {
            return "\uD83D\uDD08"
        }
        return "\uD83D\uDD0A"
    }

    component SmallTransportButton: ToolButton {
        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        bottomPadding: 0
        implicitWidth: 40
        implicitHeight: 40
        property bool accentActive: false

        background: Rectangle {
            implicitWidth: 40
            implicitHeight: 40
            radius: 20
            color: root.controlBgColor(parent.enabled, parent.accentActive, parent.hovered)
            border.width: 1
            border.color: parent.accentActive ? theme.accentStrong : theme.stroke
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 10

        RowLayout {
            Layout.fillWidth: true
            spacing: 14

            Rectangle {
                Layout.preferredWidth: 64
                Layout.preferredHeight: 64
                radius: 16
                color: theme.panelRaised
                border.width: 1
                border.color: theme.stroke

                Image {
                    anchors.fill: parent
                    anchors.margins: 1
                    asynchronous: true
                    cache: false
                    fillMode: Image.PreserveAspectCrop
                    sourceSize.width: 64
                    sourceSize.height: 64
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

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 64

                Text {
                    anchors.left: parent.left
                    anchors.right: controlGroup.left
                    anchors.rightMargin: 18
                    anchors.verticalCenter: parent.verticalCenter
                    text: playback.hasTrack ? playback.title : "Nada tocando"
                    color: theme.text
                    font.pixelSize: 18
                    font.weight: Font.Black
                    elide: Text.ElideRight
                }

                Row {
                    id: controlGroup
                    anchors.centerIn: parent
                    spacing: 8

                    SmallTransportButton {
                        id: previousButton
                        enabled: playback.canGoPrevious
                        hoverEnabled: enabled
                        onClicked: playback.previous()

                        contentItem: Image {
                            anchors.centerIn: parent
                            width: 18
                            height: 18
                            sourceSize.width: 18
                            sourceSize.height: 18
                            source: "qrc:/qt/qml/MyFin/src/ui/qml/assets/icons/previous.svg"
                            opacity: previousButton.enabled ? 1.0 : 0.38
                        }
                    }

                    ToolButton {
                        id: playButton
                        enabled: playback.hasTrack
                        hoverEnabled: enabled
                        onClicked: playback.togglePlaying()
                        leftPadding: 0
                        rightPadding: 0
                        topPadding: 0
                        bottomPadding: 0

                        background: Rectangle {
                            implicitWidth: 58
                            implicitHeight: 58
                            radius: 29
                            color: root.controlBgColor(playButton.enabled, true, playButton.hovered)
                            border.width: 1
                            border.color: theme.stroke
                        }

                        contentItem: Item {
                            implicitWidth: 28
                            implicitHeight: 28

                            Image {
                                anchors.centerIn: parent
                                width: 24
                                height: 24
                                sourceSize.width: 24
                                sourceSize.height: 24
                                source: playback.playing
                                        ? "qrc:/qt/qml/MyFin/src/ui/qml/assets/icons/pause.svg"
                                        : "qrc:/qt/qml/MyFin/src/ui/qml/assets/icons/play.svg"
                                opacity: playButton.enabled ? 1.0 : 0.38
                            }
                        }
                    }

                    SmallTransportButton {
                        id: nextButton
                        enabled: playback.canGoNext
                        hoverEnabled: enabled
                        onClicked: playback.next()

                        contentItem: Image {
                            anchors.centerIn: parent
                            width: 18
                            height: 18
                            sourceSize.width: 18
                            sourceSize.height: 18
                            source: "qrc:/qt/qml/MyFin/src/ui/qml/assets/icons/next.svg"
                            opacity: nextButton.enabled ? 1.0 : 0.38
                        }
                    }
                }

                    ToolButton {
                        id: repeatButton
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    enabled: playback.hasTrack
                    hoverEnabled: enabled
                    onClicked: playback.toggleRepeatCurrent()
                    leftPadding: 0
                    rightPadding: 0
                    topPadding: 0
                    bottomPadding: 0
                    implicitWidth: 36
                    implicitHeight: 36

                    background: Rectangle {
                        implicitWidth: 36
                        implicitHeight: 36
                        radius: 18
                        color: root.controlBgColor(repeatButton.enabled, playback.repeatCurrent, repeatButton.hovered)
                        border.width: 1
                        border.color: playback.repeatCurrent ? theme.accentStrong : theme.stroke
                    }

                    contentItem: Item {
                        implicitWidth: 18
                        implicitHeight: 18

                        Image {
                            anchors.centerIn: parent
                            width: 18
                            height: 18
                            sourceSize.width: 18
                            sourceSize.height: 18
                            source: "qrc:/qt/qml/MyFin/src/ui/qml/assets/icons/repeat-one.svg"
                            opacity: repeatButton.enabled ? 1.0 : 0.38
                        }

                        Text {
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                            anchors.rightMargin: -1
                            anchors.bottomMargin: -1
                            text: "1"
                            color: root.controlFgColor(repeatButton.enabled, playback.repeatCurrent)
                            font.pixelSize: 8
                            font.weight: Font.Black
                            visible: playback.repeatCurrent
                        }
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
                onPressedChanged: if (!pressed) playback.seek(Math.round(value))

                background: Rectangle {
                    x: progressSlider.leftPadding
                    y: progressSlider.topPadding + progressSlider.availableHeight / 2 - height / 2
                    width: progressSlider.availableWidth
                    height: 5
                    radius: 3
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

            RowLayout {
                spacing: 8
                Layout.leftMargin: 8
                Layout.preferredWidth: root.volumeClusterWidth
                Layout.alignment: Qt.AlignRight

                Text {
                    text: root.volumeIcon(playback.outputVolume)
                    color: theme.textMuted
                    font.pixelSize: 15
                    Layout.alignment: Qt.AlignVCenter
                    Layout.preferredWidth: 20
                    horizontalAlignment: Text.AlignHCenter
                }

                Slider {
                    id: volumeSlider
                    Layout.fillWidth: true
                    from: 0
                    to: 1
                    value: playback.outputVolume
                    onMoved: playback.setOutputVolume(value)

                    background: Rectangle {
                        x: volumeSlider.leftPadding
                        y: volumeSlider.topPadding + volumeSlider.availableHeight / 2 - height / 2
                        width: volumeSlider.availableWidth
                        height: 4
                        radius: 2
                        color: theme.panelRaised

                        Rectangle {
                            width: volumeSlider.visualPosition * parent.width
                            height: parent.height
                            radius: parent.radius
                            color: theme.warm
                        }
                    }

                    handle: Rectangle {
                        x: volumeSlider.leftPadding + volumeSlider.visualPosition * (volumeSlider.availableWidth - width)
                        y: volumeSlider.topPadding + volumeSlider.availableHeight / 2 - height / 2
                        width: 12
                        height: 12
                        radius: 6
                        color: theme.text
                        border.width: 1
                        border.color: theme.stroke
                    }
                }
            }
        }
    }
}
