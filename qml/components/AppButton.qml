import QtQuick
import QtQuick.Controls

Button {
    id: control

    property color accentColor: "#d99f5d"
    property color textColor: "#f5f7fb"
    property bool subtle: false

    font.pixelSize: 14
    padding: 14

    background: Rectangle {
        radius: 16
        color: control.subtle ? Qt.rgba(1, 1, 1, control.down ? 0.18 : 0.10)
                              : (control.down ? Qt.darker(control.accentColor, 1.15) : control.accentColor)
        border.width: control.subtle ? 1 : 0
        border.color: Qt.rgba(1, 1, 1, 0.15)

        Behavior on color {
            ColorAnimation { duration: 180 }
        }
    }

    contentItem: Text {
        text: control.text
        color: control.textColor
        font: control.font
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
