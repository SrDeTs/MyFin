import QtQuick

Rectangle {
    id: root

    property url source
    property color accentColor: "#d99f5d"
    property string fallbackTitle: "No Cover"

    radius: 26
    color: Qt.rgba(1, 1, 1, 0.08)
    border.width: 1
    border.color: Qt.rgba(1, 1, 1, 0.08)
    clip: true

    Image {
        anchors.fill: parent
        source: root.source
        fillMode: Image.PreserveAspectCrop
        asynchronous: true
        cache: true
        visible: status === Image.Ready
    }

    Rectangle {
        anchors.fill: parent
        visible: !parent.children[0].visible
        gradient: Gradient {
            GradientStop { position: 0.0; color: Qt.lighter(root.accentColor, 1.15) }
            GradientStop { position: 1.0; color: Qt.darker(root.accentColor, 1.65) }
        }
    }

    Text {
        anchors.centerIn: parent
        visible: !parent.children[0].visible
        text: root.fallbackTitle
        color: "#f5f7fb"
        font.pixelSize: Math.max(14, parent.width * 0.08)
        font.bold: true
    }
}
