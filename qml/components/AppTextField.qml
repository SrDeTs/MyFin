import QtQuick
import QtQuick.Controls

TextField {
    id: control

    property color accentColor: "#d99f5d"
    property color textColor: "#f5f7fb"

    implicitHeight: 46
    color: textColor
    placeholderTextColor: Qt.rgba(1, 1, 1, 0.34)
    font.pixelSize: 14
    leftPadding: 16
    rightPadding: 16
    topPadding: 12
    bottomPadding: 12
    selectByMouse: true
    selectionColor: Qt.rgba(accentColor.r, accentColor.g, accentColor.b, 0.42)
    selectedTextColor: "#f8fbff"

    background: Rectangle {
        radius: 14
        color: Qt.rgba(0.05, 0.06, 0.09, control.activeFocus ? 0.88 : 0.70)
        border.width: 1
        border.color: control.activeFocus
                      ? Qt.rgba(control.accentColor.r, control.accentColor.g, control.accentColor.b, 0.72)
                      : Qt.rgba(1, 1, 1, 0.09)

        Behavior on border.color {
            ColorAnimation { duration: 150 }
        }
    }
}
