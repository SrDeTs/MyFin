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
                    implicitHeight: accountCardContent.implicitHeight + 36
                    radius: theme.radiusMedium
                    color: theme.panelMuted
                    border.width: 1
                    border.color: theme.stroke

                    Column {
                        id: accountCardContent
                        width: parent.width - 36
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.margins: 18
                        spacing: 14

                        Text {
                            text: "Conta Jellyfin"
                            color: theme.text
                            font.pixelSize: 18
                            font.weight: Font.DemiBold
                        }

                        Text {
                            width: parent.width
                            text: appVm.session.authenticated
                                  ? "Sessao atual: " + appVm.session.summary
                                  : "Nenhuma sessao ativa no momento"
                            color: appVm.session.authenticated ? theme.accentStrong : theme.textMuted
                            font.pixelSize: 13
                            wrapMode: Text.WordWrap
                        }

                        Column {
                            width: parent.width
                            spacing: 10

                            Text {
                                text: "Servidor"
                                color: theme.textMuted
                                font.pixelSize: 12
                                font.weight: Font.DemiBold
                            }

                            TextField {
                                width: parent.width
                                implicitHeight: 42
                                text: appVm.session.serverUrl
                                placeholderText: "http://192.168.1.71:8096"
                                enabled: !appVm.session.busy
                                color: theme.text
                                placeholderTextColor: theme.textMuted
                                leftPadding: 14
                                rightPadding: 14
                                topPadding: 0
                                bottomPadding: 0
                                verticalAlignment: TextInput.AlignVCenter
                                selectByMouse: true
                                onTextEdited: appVm.session.serverUrl = text

                                background: Rectangle {
                                    radius: 14
                                    color: theme.panelRaised
                                    border.width: 1
                                    border.color: parent.activeFocus ? theme.accentStrong : theme.stroke
                                }
                            }
                        }

                        Column {
                            width: parent.width
                            spacing: 10

                            Text {
                                text: "Usuario"
                                color: theme.textMuted
                                font.pixelSize: 12
                                font.weight: Font.DemiBold
                            }

                            TextField {
                                width: parent.width
                                implicitHeight: 42
                                text: appVm.session.username
                                placeholderText: "Michael"
                                enabled: !appVm.session.busy
                                color: theme.text
                                placeholderTextColor: theme.textMuted
                                leftPadding: 14
                                rightPadding: 14
                                topPadding: 0
                                bottomPadding: 0
                                verticalAlignment: TextInput.AlignVCenter
                                selectByMouse: true
                                onTextEdited: appVm.session.username = text

                                background: Rectangle {
                                    radius: 14
                                    color: theme.panelRaised
                                    border.width: 1
                                    border.color: parent.activeFocus ? theme.accentStrong : theme.stroke
                                }
                            }
                        }

                        Column {
                            width: parent.width
                            spacing: 10

                            Text {
                                text: "Senha"
                                color: theme.textMuted
                                font.pixelSize: 12
                                font.weight: Font.DemiBold
                            }

                            TextField {
                                width: parent.width
                                implicitHeight: 42
                                text: appVm.session.password
                                placeholderText: appVm.session.authenticated
                                                 ? "Digite uma nova senha para reconectar"
                                                 : "Senha"
                                echoMode: TextInput.Password
                                enabled: !appVm.session.busy
                                color: theme.text
                                placeholderTextColor: theme.textMuted
                                leftPadding: 14
                                rightPadding: 14
                                topPadding: 0
                                bottomPadding: 0
                                verticalAlignment: TextInput.AlignVCenter
                                selectByMouse: true
                                onTextEdited: appVm.session.password = text
                                onAccepted: appVm.session.login()

                                background: Rectangle {
                                    radius: 14
                                    color: theme.panelRaised
                                    border.width: 1
                                    border.color: parent.activeFocus ? theme.accentStrong : theme.stroke
                                }
                            }
                        }

                        Text {
                            visible: appVm.session.errorText.length > 0
                            width: parent.width
                            text: appVm.session.errorText
                            color: theme.danger
                            font.pixelSize: 12
                            wrapMode: Text.WordWrap
                        }
                        Row {
                            width: parent.width
                            spacing: 10

                            Button {
                                id: connectButton
                                width: 140
                                text: appVm.session.authenticated ? "Reconectar" : "Conectar"
                                enabled: !appVm.session.busy
                                onClicked: appVm.session.login()

                                background: Rectangle {
                                    radius: 14
                                    color: connectButton.down ? theme.warm : theme.accentStrong
                                    border.width: 1
                                    border.color: theme.stroke
                                }

                                contentItem: Text {
                                    text: connectButton.text
                                    color: theme.window
                                    font.pixelSize: 13
                                    font.weight: Font.DemiBold
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }

                            Button {
                                id: refreshButton
                                visible: appVm.session.authenticated
                                width: 180
                                text: "Atualizar biblioteca"
                                enabled: !appVm.session.busy
                                onClicked: appVm.reloadLibrary()

                                background: Rectangle {
                                    radius: 14
                                    color: refreshButton.down ? theme.panel : theme.panelRaised
                                    border.width: 1
                                    border.color: theme.stroke
                                }

                                contentItem: Text {
                                    text: refreshButton.text
                                    color: theme.text
                                    font.pixelSize: 13
                                    font.weight: Font.DemiBold
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }

                            Button {
                                id: disconnectButton
                                visible: appVm.session.authenticated
                                width: 130
                                text: "Desconectar"
                                enabled: !appVm.session.busy
                                onClicked: appVm.session.logout()

                                background: Rectangle {
                                    radius: 14
                                    color: disconnectButton.down ? theme.panel : theme.panelRaised
                                    border.width: 1
                                    border.color: theme.stroke
                                }

                                contentItem: Text {
                                    text: disconnectButton.text
                                    color: theme.text
                                    font.pixelSize: 13
                                    font.weight: Font.DemiBold
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }
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
