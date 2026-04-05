import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import MyFin

Item {
    Column {
        anchors.fill: parent
        spacing: 18

        Text {
            text: "Configuracoes"
            color: AppController.theme.textColor
            font.pixelSize: 28
            font.bold: true
        }

        Flickable {
            width: parent.width
            height: parent.height - 46
            contentWidth: width
            contentHeight: contentColumn.height
            clip: true

            Column {
                id: contentColumn
                width: parent.width
                spacing: 18

                Rectangle {
                    width: parent.width
                    height: 304
                    radius: 28
                    color: Qt.rgba(1, 1, 1, 0.06)

                    ColumnLayout {
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
                            text: "Servidor, usuario e senha ficam salvos localmente nas configuracoes do app."
                            color: Qt.rgba(1, 1, 1, 0.56)
                            font.pixelSize: 13
                            Layout.fillWidth: true
                            wrapMode: Text.WordWrap
                        }

                        AppTextField {
                            Layout.fillWidth: true
                            text: AppController.settings.jellyfinUrl
                            placeholderText: "https://seu-servidor:8096"
                            onTextEdited: AppController.settings.jellyfinUrl = text
                            accentColor: AppController.theme.accentColor
                            textColor: AppController.theme.textColor
                        }

                        AppTextField {
                            Layout.fillWidth: true
                            text: AppController.settings.jellyfinUsername
                            placeholderText: "Usuario"
                            onTextEdited: AppController.settings.jellyfinUsername = text
                            accentColor: AppController.theme.accentColor
                            textColor: AppController.theme.textColor
                        }

                        AppTextField {
                            Layout.fillWidth: true
                            text: AppController.settings.jellyfinPassword
                            placeholderText: "Senha"
                            echoMode: TextInput.Password
                            onTextEdited: AppController.settings.jellyfinPassword = text
                            accentColor: AppController.theme.accentColor
                            textColor: AppController.theme.textColor
                        }

                        RowLayout {
                            Layout.fillWidth: true
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
                                width: 120
                                height: 44
                                text: "Salvar"
                                subtle: true
                                accentColor: AppController.theme.accentColor
                                textColor: AppController.theme.textColor
                                onClicked: AppController.settings.save()
                            }

                            Item {
                                Layout.fillWidth: true
                            }
                        }

                        Text {
                            text: AppController.jellyfin.errorString.length > 0
                                  ? AppController.jellyfin.errorString
                                  : (AppController.jellyfin.connected ? "Conexao ativa." : "Sem conexao ativa.")
                            color: AppController.jellyfin.errorString.length > 0 ? "#ffd4d4" : Qt.rgba(1, 1, 1, 0.68)
                            Layout.fillWidth: true
                            wrapMode: Text.WordWrap
                        }
                    }
                }

                Rectangle {
                    width: parent.width
                    height: 446
                    radius: 28
                    color: Qt.rgba(1, 1, 1, 0.06)

                    ColumnLayout {
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

                        AppTextField {
                            Layout.fillWidth: true
                            text: AppController.settings.musicDirectoriesDisplay()
                            placeholderText: "/home/user/Music; /mnt/media/music"
                            onTextEdited: AppController.settings.setMusicDirectoriesFromString(text)
                            accentColor: AppController.theme.accentColor
                            textColor: AppController.theme.textColor
                        }

                        GridLayout {
                            Layout.fillWidth: true
                            columns: 2
                            rowSpacing: 14
                            columnSpacing: 18

                            Text {
                                text: "Volume padrao"
                                color: AppController.theme.textColor
                                font.pixelSize: 14
                            }

                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 12

                                Slider {
                                    Layout.fillWidth: true
                                    from: 0
                                    to: 100
                                    value: AppController.settings.defaultVolume
                                    onMoved: AppController.settings.defaultVolume = value
                                }

                                Text {
                                    text: Math.round(AppController.settings.defaultVolume) + "%"
                                    color: Qt.rgba(1, 1, 1, 0.68)
                                    font.pixelSize: 13
                                }
                            }

                            Text {
                                text: "Animacoes"
                                color: AppController.theme.textColor
                                font.pixelSize: 14
                            }

                            AppSwitch {
                                checked: AppController.settings.animationsEnabled
                                accentColor: AppController.theme.accentColor
                                onToggled: AppController.settings.animationsEnabled = checked
                            }

                            Text {
                                text: "Tela inicial"
                                color: AppController.theme.textColor
                                font.pixelSize: 14
                            }

                            AppComboBox {
                                Layout.preferredWidth: 220
                                model: ["home", "library", "nowplaying"]
                                currentIndex: model.indexOf(AppController.settings.startupBehavior)
                                accentColor: AppController.theme.accentColor
                                textColor: AppController.theme.textColor
                                onActivated: AppController.settings.startupBehavior = currentText
                            }

                            Text {
                                text: "Fila"
                                color: AppController.theme.textColor
                                font.pixelSize: 14
                            }

                            AppComboBox {
                                Layout.preferredWidth: 220
                                model: ["append", "replace"]
                                currentIndex: model.indexOf(AppController.settings.queueBehavior)
                                accentColor: AppController.theme.accentColor
                                textColor: AppController.theme.textColor
                                onActivated: AppController.settings.queueBehavior = currentText
                            }

                            Text {
                                text: "Tema visual"
                                color: AppController.theme.textColor
                                font.pixelSize: 14
                            }

                            AppComboBox {
                                Layout.preferredWidth: 220
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

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 12

                            AppButton {
                                width: 120
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

                            Item {
                                Layout.fillWidth: true
                            }
                        }
                    }
                }
            }
        }
    }
}
