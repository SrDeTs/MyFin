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

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 3

                Text {
                    text: playback.hasTrack ? playback.title : "Nada tocando"
                    color: theme.text
                    font.pixelSize: 18
                    font.weight: Font.Black
                    elide: Text.ElideRight
                }

                Item {
                    implicitHeight: 0
                }
            }

            RowLayout {
                spacing: 8

                ToolButton {
                    id: previousButton
                    enabled: playback.canGoPrevious
                    hoverEnabled: enabled
                    onClicked: playback.previous()
                    leftPadding: 0
                    rightPadding: 0
                    topPadding: 0
                    bottomPadding: 0

                    background: Rectangle {
                        implicitWidth: 40
                        implicitHeight: 40
                        radius: 20
                        color: root.controlBgColor(previousButton.enabled, false, previousButton.hovered)
                        border.width: 1
                        border.color: theme.stroke
                    }

                    contentItem: Text {
                        text: "\u23EE"
                        color: root.controlFgColor(previousButton.enabled, false)
                        font.pixelSize: 16
                        font.weight: Font.Black
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
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

                        Canvas {
                            id: playGlyph
                            anchors.centerIn: parent
                            width: 24
                            height: 24
                            visible: !playback.playing

                            onPaint: {
                                const ctx = getContext("2d")
                                ctx.reset()
                                ctx.fillStyle = playButton.enabled ? theme.panelMuted : theme.textMuted
                                ctx.beginPath()
                                ctx.moveTo(6, 3)
                                ctx.lineTo(20, 12)
                                ctx.lineTo(6, 21)
                                ctx.closePath()
                                ctx.fill()
                            }

                            Component.onCompleted: requestPaint()
                            onVisibleChanged: requestPaint()
                            Connections {
                                target: playButton
                                function onEnabledChanged() { playGlyph.requestPaint() }
                            }
                        }

                        Row {
                            anchors.centerIn: parent
                            spacing: 5
                            visible: playback.playing

                            Rectangle {
                                width: 5
                                height: 18
                                radius: 1
                                color: playButton.enabled ? theme.panelMuted : theme.textMuted
                            }

                            Rectangle {
                                width: 5
                                height: 18
                                radius: 1
                                color: playButton.enabled ? theme.panelMuted : theme.textMuted
                            }
                        }
                    }
                }

                ToolButton {
                    id: nextButton
                    enabled: playback.canGoNext
                    hoverEnabled: enabled
                    onClicked: playback.next()
                    leftPadding: 0
                    rightPadding: 0
                    topPadding: 0
                    bottomPadding: 0

                    background: Rectangle {
                        implicitWidth: 40
                        implicitHeight: 40
                        radius: 20
                        color: root.controlBgColor(nextButton.enabled, false, nextButton.hovered)
                        border.width: 1
                        border.color: theme.stroke
                    }

                    contentItem: Text {
                        text: "\u23ED"
                        color: root.controlFgColor(nextButton.enabled, false)
                        font.pixelSize: 16
                        font.weight: Font.Black
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

                Text {
                    text: root.volumeIcon(playback.outputVolume)
                    color: theme.textMuted
                    font.pixelSize: 15
                    Layout.alignment: Qt.AlignVCenter
                }

                Slider {
                    id: volumeSlider
                    Layout.preferredWidth: 110
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
