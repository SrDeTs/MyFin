import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    required property var appVm
    required property var theme

    Rectangle {
        anchors.fill: parent
        radius: theme.radiusLarge
        color: theme.panel
        border.width: 1
        border.color: theme.stroke

        Flickable {
            anchors.fill: parent
            anchors.margins: 18
            contentWidth: width
            contentHeight: contentColumn.implicitHeight
            clip: true

            Column {
                id: contentColumn
                width: Math.min(parent.width, 760)
                x: Math.max(0, (parent.width - width) / 2)
                spacing: 18

                Rectangle {
                    width: parent.width
                    implicitHeight: libraryCardContent.implicitHeight + 36
                    radius: theme.radiusMedium
                    color: theme.panelMuted
                    border.width: 1
                    border.color: theme.stroke

                    Column {
                        id: libraryCardContent
                        width: parent.width - 36
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.margins: 18
                        spacing: 14

                        Text {
                            text: "Biblioteca"
                            color: theme.text
                            font.pixelSize: 18
                            font.weight: Font.DemiBold
                        }

                        Text {
                            text: "Faixas em cache: " + appVm.library.cachedTrackCount
                            color: theme.textMuted
                            font.pixelSize: 13
                        }

                        Text {
                            text: appVm.session.authenticated
                                  ? "Servidor conectado: " + appVm.session.summary
                                  : "Nenhuma sessao conectada no momento"
                            color: appVm.session.authenticated ? theme.accentStrong : theme.textMuted
                            font.pixelSize: 13
                            wrapMode: Text.WordWrap
                        }

                        AppButton {
                            width: 180
                            theme: root.theme
                            text: "Atualizar biblioteca"
                            enabled: appVm.session.authenticated && !appVm.session.busy
                            onClicked: appVm.reloadLibrary()
                        }
                    }
                }
            }

            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AsNeeded
            }
        }
    }
}
