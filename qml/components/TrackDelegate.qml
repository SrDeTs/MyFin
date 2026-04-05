import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import MyFin

Rectangle {
    id: root

    property string title: ""
    property string artist: ""
    property string album: ""
    property string sourceName: ""
    property url artUrl: ""
    property int durationMs: 0
    property bool active: false
    property color accentColor: "#d99f5d"
    property color textColor: "#f5f7fb"
    signal playRequested()
    signal enqueueRequested()

    radius: 20
    color: active ? Qt.rgba(accentColor.r, accentColor.g, accentColor.b, 0.18)
                  : Qt.rgba(1, 1, 1, 0.06)
    border.width: active ? 1 : 0
    border.color: Qt.rgba(1, 1, 1, 0.12)
    height: 88

    RowLayout {
        anchors.fill: parent
        anchors.margins: 14
        spacing: 14

        CoverArt {
            width: 60
            height: 60
            Layout.alignment: Qt.AlignVCenter
            source: root.artUrl
            accentColor: root.accentColor
            fallbackTitle: title.length > 0 ? title[0] : "M"
        }

        Column {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            spacing: 4

            Text {
                text: root.title
                color: root.textColor
                font.pixelSize: 16
                font.bold: true
                elide: Text.ElideRight
                width: parent.width
            }

            Text {
                text: (root.artist || "Unknown Artist") + "  |  " + (root.album || "Unknown Album")
                color: Qt.rgba(1, 1, 1, 0.72)
                font.pixelSize: 13
                elide: Text.ElideRight
                width: parent.width
            }

            Text {
                text: root.sourceName
                color: Qt.rgba(1, 1, 1, 0.50)
                font.pixelSize: 12
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignVCenter
            spacing: 10

            Text {
                text: AppController.formatTime(root.durationMs)
                color: Qt.rgba(1, 1, 1, 0.66)
                font.pixelSize: 12
            }

            AppButton {
                width: 58
                height: 38
                text: "Play"
                accentColor: root.accentColor
                textColor: root.textColor
                onClicked: root.playRequested()
            }

            AppButton {
                width: 58
                height: 38
                text: "Add"
                subtle: true
                accentColor: root.accentColor
                textColor: root.textColor
                onClicked: root.enqueueRequested()
            }
        }
    }
}
