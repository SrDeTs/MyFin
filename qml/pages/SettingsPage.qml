import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import MyFin

Item {
    id: root

    Column {
        anchors.fill: parent
        spacing: 18

        Text {
            text: "Configuracoes"
            color: AppController.theme.textColor
            font.pixelSize: 28
            font.bold: true
        }

        Rectangle {
            width: parent.width
            radius: 28
            color: Qt.rgba(1, 1, 1, 0.06)
            border.width: 1
            border.color: Qt.rgba(1, 1, 1, 0.05)
            implicitHeight: jellyfinColumn.implicitHeight + 44

            Column {
                id: jellyfinColumn
                anchors.fill: parent
                anchors.margins: 22
                spacing: 12

                Text {
                    text: "Conexao Jellyfin"
                    color: AppController.theme.textColor
                    font.pixelSize: 22
                    font.bold: true
                }

                Text {
                    text: "Servidor, usuario e senha ficam salvos localmente. O ideal ainda e migrar isso para keyring."
                    color: Qt.rgba(1, 1, 1, 0.56)
                    font.pixelSize: 13
                    width: parent.width
                    wrapMode: Text.WordWrap
                }

                AppTextField {
                    width: parent.width
                    text: AppController.settings.jellyfinUrl
                    placeholderText: "https://seu-servidor:8096"
                    onTextEdited: AppController.settings.jellyfinUrl = text
                    accentColor: AppController.theme.accentColor
                    textColor: AppController.theme.textColor
                }

                AppTextField {
                    width: parent.width
                    text: AppController.settings.jellyfinUsername
                    placeholderText: "Usuario"
                    onTextEdited: AppController.settings.jellyfinUsername = text
                    accentColor: AppController.theme.accentColor
                    textColor: AppController.theme.textColor
                }

                AppTextField {
                    width: parent.width
                    text: AppController.settings.jellyfinPassword
                    placeholderText: "Senha"
                    echoMode: TextInput.Password
                    onTextEdited: AppController.settings.jellyfinPassword = text
                    accentColor: AppController.theme.accentColor
                    textColor: AppController.theme.textColor
                }

                Row {
                    spacing: 12

                    AppButton {
                        width: 128
                        height: 44
                        text: AppController.jellyfin.busy ? "Conectando" : "Conectar"
                        accentColor: AppController.theme.accentColor
                        textColor: AppController.theme.textColor
                        onClicked: AppController.connectJellyfin()
                    }

                    AppButton {
                        width: 116
                        height: 44
                        text: "Salvar"
                        subtle: true
                        accentColor: AppController.theme.accentColor
                        textColor: AppController.theme.textColor
                        onClicked: AppController.settings.save()
                    }
                }

                Text {
                    text: AppController.jellyfin.errorString.length > 0
                          ? AppController.jellyfin.errorString
                          : (AppController.jellyfin.connected ? "Conexao ativa." : "Sem conexao ativa.")
                    color: AppController.jellyfin.errorString.length > 0 ? "#ffd4d4" : Qt.rgba(1, 1, 1, 0.68)
                    font.pixelSize: 13
                    width: parent.width
                    wrapMode: Text.WordWrap
                }
            }
        }

        Rectangle {
            width: parent.width
            radius: 28
            color: Qt.rgba(1, 1, 1, 0.06)
            border.width: 1
            border.color: Qt.rgba(1, 1, 1, 0.05)
            implicitHeight: generalColumn.implicitHeight + 44

            Column {
                id: generalColumn
                anchors.fill: parent
                anchors.margins: 22
                spacing: 14

                Text {
                    text: "Configuracoes gerais"
                    color: AppController.theme.textColor
                    font.pixelSize: 22
                    font.bold: true
                }

                Text {
                    text: "Ajustes visuais e de comportamento do player."
                    color: Qt.rgba(1, 1, 1, 0.56)
                    font.pixelSize: 13
                }

                Text {
                    text: "Diretorios de musica"
                    color: AppController.theme.textColor
                    font.pixelSize: 14
                    font.bold: true
                }

                AppTextField {
                    width: parent.width
                    text: AppController.settings.musicDirectoriesDisplay()
                    placeholderText: "/home/user/Music; /mnt/media/music"
                    onTextEdited: AppController.settings.setMusicDirectoriesFromString(text)
                    accentColor: AppController.theme.accentColor
                    textColor: AppController.theme.textColor
                }

                RowLayout {
                    width: parent.width
                    spacing: 14

                    Text {
                        Layout.preferredWidth: 140
                        text: "Volume padrao"
                        color: AppController.theme.textColor
                        font.pixelSize: 14
                        font.bold: true
                    }

                    Slider {
                        id: defaultVolumeSlider
                        Layout.fillWidth: true
                        from: 0
                        to: 100
                        value: AppController.settings.defaultVolume
                        onMoved: AppController.settings.defaultVolume = Math.round(value)

                        background: Rectangle {
                            x: defaultVolumeSlider.leftPadding
                            y: defaultVolumeSlider.topPadding + defaultVolumeSlider.availableHeight / 2 - height / 2
                            width: defaultVolumeSlider.availableWidth
                            height: 6
                            radius: 3
                            color: Qt.rgba(1, 1, 1, 0.10)

                            Rectangle {
                                width: defaultVolumeSlider.visualPosition * parent.width
                                height: parent.height
                                radius: 3
                                color: AppController.theme.accentColor
                            }
                        }

                        handle: Rectangle {
                            x: defaultVolumeSlider.leftPadding + defaultVolumeSlider.visualPosition * (defaultVolumeSlider.availableWidth - width)
                            y: defaultVolumeSlider.topPadding + defaultVolumeSlider.availableHeight / 2 - height / 2
                            width: 14
                            height: 14
                            radius: 7
                            color: "#f2f5fb"
                            border.width: 2
                            border.color: AppController.theme.accentColor
                        }
                    }

                    Text {
                        Layout.preferredWidth: 42
                        text: Math.round(defaultVolumeSlider.value) + "%"
                        color: Qt.rgba(1, 1, 1, 0.68)
                        font.pixelSize: 13
                        horizontalAlignment: Text.AlignRight
                    }
                }

                Row {
                    width: parent.width
                    height: 32
                    spacing: 14

                    Text {
                        width: 140
                        anchors.verticalCenter: parent.verticalCenter
                        text: "Animacoes"
                        color: AppController.theme.textColor
                        font.pixelSize: 14
                        font.bold: true
                    }

                    AppSwitch {
                        anchors.verticalCenter: parent.verticalCenter
                        checked: AppController.settings.animationsEnabled
                        accentColor: AppController.theme.accentColor
                        onToggled: AppController.settings.animationsEnabled = checked
                    }
                }

                Row {
                    width: parent.width
                    height: 44
                    spacing: 14

                    Text {
                        width: 140
                        anchors.verticalCenter: parent.verticalCenter
                        text: "Tela inicial"
                        color: AppController.theme.textColor
                        font.pixelSize: 14
                        font.bold: true
                    }

                    AppComboBox {
                        width: 170
                        anchors.verticalCenter: parent.verticalCenter
                        model: ["home", "library", "nowplaying"]
                        currentIndex: model.indexOf(AppController.settings.startupBehavior)
                        accentColor: AppController.theme.accentColor
                        textColor: AppController.theme.textColor
                        onActivated: AppController.settings.startupBehavior = currentText
                    }
                }

                Row {
                    width: parent.width
                    height: 44
                    spacing: 14

                    Text {
                        width: 140
                        anchors.verticalCenter: parent.verticalCenter
                        text: "Fila"
                        color: AppController.theme.textColor
                        font.pixelSize: 14
                        font.bold: true
                    }

                    AppComboBox {
                        width: 170
                        anchors.verticalCenter: parent.verticalCenter
                        model: ["append", "replace"]
                        currentIndex: model.indexOf(AppController.settings.queueBehavior)
                        accentColor: AppController.theme.accentColor
                        textColor: AppController.theme.textColor
                        onActivated: AppController.settings.queueBehavior = currentText
                    }
                }

                Row {
                    width: parent.width
                    height: 44
                    spacing: 14

                    Text {
                        width: 140
                        anchors.verticalCenter: parent.verticalCenter
                        text: "Tema visual"
                        color: AppController.theme.textColor
                        font.pixelSize: 14
                        font.bold: true
                    }

                    AppComboBox {
                        width: 170
                        anchors.verticalCenter: parent.verticalCenter
                        model: ["dynamic", "fallback"]
                        currentIndex: model.indexOf(AppController.settings.visualTheme)
                        accentColor: AppController.theme.accentColor
                        textColor: AppController.theme.textColor
                        onActivated: {
                            AppController.settings.visualTheme = currentText
                            if (currentText === "fallback")
                                AppController.theme.applyFallbackTheme()
                        }
                    }
                }

                Row {
                    spacing: 12

                    AppButton {
                        width: 124
                        height: 44
                        text: "Salvar tudo"
                        accentColor: AppController.theme.accentColor
                        textColor: AppController.theme.textColor
                        onClicked: {
                            AppController.settings.save()
                            AppController.reloadLocalLibrary()
                        }
                    }

                    AppButton {
                        width: 150
                        height: 44
                        text: "Restaurar padrao"
                        subtle: true
                        accentColor: AppController.theme.accentColor
                        textColor: AppController.theme.textColor
                        onClicked: AppController.settings.restoreDefaults()
                    }
                }
            }
        }
    }
}
