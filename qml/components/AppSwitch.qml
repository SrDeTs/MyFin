import QtQuick
import QtQuick.Controls

Switch {
    id: control

    property color accentColor: "#d99f5d"

    indicator: Rectangle {
        implicitWidth: 46
        implicitHeight: 28
        radius: 14
        color: control.checked
               ? Qt.rgba(control.accentColor.r, control.accentColor.g, control.accentColor.b, 0.58)
               : Qt.rgba(1, 1, 1, 0.14)
        border.width: 1
        border.color: Qt.rgba(1, 1, 1, 0.10)

        Rectangle {
            x: control.checked ? parent.width - width - 3 : 3
            y: 3
            width: 22
            height: 22
            radius: 11
            color: "#f4f6fb"

            Behavior on x {
                NumberAnimation { duration: 140; easing.type: Easing.OutCubic }
            }
        }
    }

    contentItem: Item {}
}
