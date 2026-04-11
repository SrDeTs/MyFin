import QtQuick
import QtQuick.Controls

Button {
    id: control

    required property var theme
    property bool active: false

    implicitHeight: 46
    implicitWidth: 180
    hoverEnabled: true

    font.pixelSize: 14
    font.weight: Font.DemiBold
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    background: Rectangle {
        radius: 14
        color: control.active
               ? control.theme.panelRaised
               : (control.hovered ? control.theme.panelMuted : "transparent")
        border.width: 1
        border.color: control.active ? control.theme.accent : "transparent"

        Rectangle {
            visible: control.active
            width: 3
            height: parent.height - 16
            radius: 2
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            color: control.theme.accentStrong
        }
    }

    contentItem: Text {
        text: control.text
        color: control.active ? control.theme.text : control.theme.textMuted
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        elide: Text.ElideRight
        leftPadding: control.active ? 24 : 16
        rightPadding: 14
        font: control.font
    }
}
