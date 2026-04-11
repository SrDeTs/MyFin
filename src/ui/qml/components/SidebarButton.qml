import QtQuick
import QtQuick.Controls

Button {
    id: control

    property bool active: false

    implicitHeight: 48
    implicitWidth: 180

    font.pixelSize: 15
    font.weight: Font.DemiBold

    background: Rectangle {
        radius: 14
        color: control.active ? "#25343a" : "transparent"
        border.width: control.active ? 1 : 0
        border.color: "#35515b"
    }

    contentItem: Text {
        text: control.text
        color: control.active ? "#f3efe8" : "#94a0aa"
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        leftPadding: 14
        rightPadding: 14
        font: control.font
    }
}
