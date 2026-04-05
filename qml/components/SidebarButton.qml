import QtQuick
import QtQuick.Controls

Button {
    id: control

    property bool active: false
    property color accentColor: "#d99f5d"
    property color textColor: "#f5f7fb"

    font.pixelSize: 15
    leftPadding: 16
    rightPadding: 16
    topPadding: 14
    bottomPadding: 14

    background: Rectangle {
        radius: 18
        color: control.active ? Qt.rgba(control.accentColor.r, control.accentColor.g, control.accentColor.b, 0.24)
                              : (control.hovered ? Qt.rgba(1, 1, 1, 0.08) : "transparent")
        border.width: control.active ? 1 : 0
        border.color: Qt.rgba(1, 1, 1, 0.15)

        Behavior on color {
            ColorAnimation { duration: 180 }
        }
    }

    contentItem: Text {
        text: control.text
        color: control.textColor
        font: control.font
        verticalAlignment: Text.AlignVCenter
    }
}
