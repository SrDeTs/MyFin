import QtQuick
import QtQuick.Controls

TextField {
    id: control

    required property var theme

    implicitHeight: 42
    color: theme.text
    placeholderTextColor: theme.textMuted
    leftPadding: 14
    rightPadding: 14
    topPadding: 0
    bottomPadding: 0
    verticalAlignment: TextInput.AlignVCenter
    selectByMouse: true

    background: Rectangle {
        radius: 14
        color: theme.panelRaised
        border.width: 1
        border.color: control.activeFocus ? theme.accentStrong : theme.stroke
    }
}
