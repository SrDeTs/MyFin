import QtQuick
import QtQuick.Controls

Button {
    id: control

    required property var theme
    property bool primary: false
    property bool danger: false

    implicitHeight: 42
    implicitWidth: 140
    hoverEnabled: enabled

    font.pixelSize: 13
    font.weight: Font.DemiBold

    background: Rectangle {
        radius: 14
        border.width: 1
        border.color: control.primary ? control.theme.accentStrong : control.theme.stroke

        color: {
            if (!control.enabled) {
                return control.theme.panelMuted
            }
            if (control.primary) {
                return control.down ? control.theme.warm : (control.hovered ? control.theme.warm : control.theme.accentStrong)
            }
            if (control.danger) {
                return control.down ? control.theme.panel : (control.hovered ? control.theme.panel : control.theme.panelRaised)
            }
            return control.down ? control.theme.panel : (control.hovered ? control.theme.panel : control.theme.panelRaised)
        }
    }

    contentItem: Text {
        text: control.text
        color: control.primary ? control.theme.window : control.theme.text
        font: control.font
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }
}
