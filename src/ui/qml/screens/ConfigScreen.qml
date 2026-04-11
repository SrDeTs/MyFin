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
                width: Math.min(parent.width, 820)
                x: Math.max(0, (parent.width - width) / 2)
                spacing: 18

                Rectangle {
                    width: parent.width
                    implicitHeight: modeCardContent.implicitHeight + 36
                    radius: theme.radiusMedium
                    color: theme.panelMuted
                    border.width: 1
                    border.color: theme.stroke

                    Column {
                        id: modeCardContent
                        width: parent.width - 36
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.margins: 18
                        spacing: 14

                        Text {
                            text: "Audio"
                            color: theme.text
                            font.pixelSize: 18
                            font.weight: Font.DemiBold
                        }

                        RowLayout {
                            width: parent.width
                            spacing: 16

                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 4

                                Text {
                                    text: "Modo avancado"
                                    color: theme.text
                                    font.pixelSize: 14
                                    font.weight: Font.DemiBold
                                }

                                Text {
                                    text: appVm.audio.advancedMode
                                          ? "Exibe controles tecnicos detalhados."
                                          : "Exibe perfis simples e saida principal."
                                    color: theme.textMuted
                                    font.pixelSize: 12
                                    wrapMode: Text.WordWrap
                                }
                            }

                            Switch {
                                checked: appVm.audio.advancedMode
                                onToggled: appVm.audio.advancedMode = checked
                            }
                        }
                    }
                }

                Rectangle {
                    width: parent.width
                    implicitHeight: outputCardContent.implicitHeight + 36
                    radius: theme.radiusMedium
                    color: theme.panelMuted
                    border.width: 1
                    border.color: theme.stroke

                    Column {
                        id: outputCardContent
                        width: parent.width - 36
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.margins: 18
                        spacing: 14

                        Text {
                            text: "Saida"
                            color: theme.text
                            font.pixelSize: 18
                            font.weight: Font.DemiBold
                        }

                        Column {
                            width: parent.width
                            spacing: 10

                            Text {
                                text: "Dispositivo de saida"
                                color: theme.textMuted
                                font.pixelSize: 12
                                font.weight: Font.DemiBold
                            }

                            ComboBox {
                                id: outputDeviceCombo
                                width: parent.width
                                model: appVm.audio.outputDevices
                                textRole: "name"
                                currentIndex: appVm.audio.currentOutputDeviceIndex
                                enabled: model.length > 0
                                onActivated: appVm.audio.selectOutputDevice(currentIndex)
                            }
                        }

                        GridLayout {
                            width: parent.width
                            columns: 2
                            rowSpacing: 10
                            columnSpacing: 18

                            Text {
                                text: "Backend"
                                color: theme.textMuted
                                font.pixelSize: 12
                                font.weight: Font.DemiBold
                            }

                            Text {
                                text: appVm.audio.backendName
                                color: theme.text
                                font.pixelSize: 13
                            }

                            Text {
                                text: "Device atual"
                                color: theme.textMuted
                                font.pixelSize: 12
                                font.weight: Font.DemiBold
                            }

                            Text {
                                text: appVm.audio.currentOutputDeviceName
                                color: theme.text
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                            }

                            Text {
                                text: "Formato"
                                color: theme.textMuted
                                font.pixelSize: 12
                                font.weight: Font.DemiBold
                            }

                            Text {
                                text: appVm.audio.outputFormatSummary
                                color: theme.text
                                font.pixelSize: 13
                            }

                            Text {
                                text: "Confianca"
                                color: theme.textMuted
                                font.pixelSize: 12
                                font.weight: Font.DemiBold
                            }

                            Text {
                                text: appVm.audio.signalConfidence
                                color: theme.accentStrong
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                            }
                        }
                    }
                }

                Rectangle {
                    width: parent.width
                    implicitHeight: qualityCardContent.implicitHeight + 36
                    radius: theme.radiusMedium
                    color: theme.panelMuted
                    border.width: 1
                    border.color: theme.stroke

                    Column {
                        id: qualityCardContent
                        width: parent.width - 36
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.margins: 18
                        spacing: 14

                        Text {
                            text: "Qualidade"
                            color: theme.text
                            font.pixelSize: 18
                            font.weight: Font.DemiBold
                        }

                        Column {
                            width: parent.width
                            spacing: 10

                            Text {
                                text: "Perfil"
                                color: theme.textMuted
                                font.pixelSize: 12
                                font.weight: Font.DemiBold
                            }

                            ComboBox {
                                width: parent.width
                                model: appVm.audio.qualityProfileOptions
                                currentIndex: appVm.audio.qualityProfileIndex
                                onActivated: appVm.audio.setQualityProfileIndex(currentIndex)
                            }
                        }

                        Column {
                            visible: appVm.audio.advancedMode
                            width: parent.width
                            spacing: 12

                            Column {
                                width: parent.width
                                spacing: 8

                                RowLayout {
                                    width: parent.width
                                    spacing: 14

                                    Text {
                                        text: "ReplayGain"
                                        color: theme.textMuted
                                        font.pixelSize: 12
                                        font.weight: Font.DemiBold
                                        Layout.preferredWidth: 110
                                    }

                                    ComboBox {
                                        Layout.fillWidth: true
                                        model: appVm.audio.replayGainOptions
                                        currentIndex: appVm.audio.replayGainIndex
                                        onActivated: appVm.audio.setReplayGainIndex(currentIndex)
                                    }
                                }

                                Text {
                                    width: parent.width
                                    text: "Ajusta o volume percebido entre faixas para evitar saltos bruscos."
                                    color: theme.textMuted
                                    font.pixelSize: 12
                                    wrapMode: Text.WordWrap
                                }
                            }

                            Column {
                                width: parent.width
                                spacing: 8

                                RowLayout {
                                    width: parent.width
                                    spacing: 14

                                    Text {
                                        text: "Gapless"
                                        color: theme.textMuted
                                        font.pixelSize: 12
                                        font.weight: Font.DemiBold
                                        Layout.preferredWidth: 110
                                    }

                                    Switch {
                                        checked: appVm.audio.gaplessEnabled
                                        onToggled: appVm.audio.gaplessEnabled = checked
                                    }
                                }

                                Text {
                                    width: parent.width
                                    text: "Tenta trocar para a proxima faixa sem silencio entre elas."
                                    color: theme.textMuted
                                    font.pixelSize: 12
                                    wrapMode: Text.WordWrap
                                }
                            }

                            Column {
                                width: parent.width
                                spacing: 8

                                RowLayout {
                                    width: parent.width
                                    spacing: 14

                                    Text {
                                        text: "Crossfade"
                                        color: theme.textMuted
                                        font.pixelSize: 12
                                        font.weight: Font.DemiBold
                                        Layout.preferredWidth: 110
                                    }

                                    Slider {
                                        id: crossfadeSlider
                                        Layout.fillWidth: true
                                        from: 0
                                        to: 12
                                        stepSize: 1
                                        value: appVm.audio.crossfadeSeconds
                                        onMoved: appVm.audio.crossfadeSeconds = Math.round(value)
                                    }

                                    Text {
                                        text: appVm.audio.crossfadeSeconds + "s"
                                        color: theme.text
                                        font.pixelSize: 13
                                        font.weight: Font.DemiBold
                                        Layout.preferredWidth: 36
                                        horizontalAlignment: Text.AlignRight
                                    }
                                }

                                Text {
                                    width: parent.width
                                    text: "Mistura o final de uma faixa com o inicio da proxima. Ainda nao implementado no motor."
                                    color: theme.textMuted
                                    font.pixelSize: 12
                                    wrapMode: Text.WordWrap
                                }
                            }

                            Column {
                                width: parent.width
                                spacing: 8

                                RowLayout {
                                    width: parent.width
                                    spacing: 14

                                    Text {
                                        text: "Preload"
                                        color: theme.textMuted
                                        font.pixelSize: 12
                                        font.weight: Font.DemiBold
                                        Layout.preferredWidth: 110
                                    }

                                    Switch {
                                        checked: appVm.audio.preloadNextTrack
                                        onToggled: appVm.audio.preloadNextTrack = checked
                                    }
                                }

                                Text {
                                    width: parent.width
                                    text: "Prepara a proxima faixa antes do fim da atual para reduzir atraso na troca."
                                    color: theme.textMuted
                                    font.pixelSize: 12
                                    wrapMode: Text.WordWrap
                                }
                            }
                        }
                    }
                }

                Rectangle {
                    width: parent.width
                    implicitHeight: cacheCardContent.implicitHeight + 36
                    radius: theme.radiusMedium
                    color: theme.panelMuted
                    border.width: 1
                    border.color: theme.stroke

                    Column {
                        id: cacheCardContent
                        width: parent.width - 36
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.margins: 18
                        spacing: 14

                        Text {
                            text: "Cache e diagnostico"
                            color: theme.text
                            font.pixelSize: 18
                            font.weight: Font.DemiBold
                        }

                        GridLayout {
                            width: parent.width
                            columns: 2
                            rowSpacing: 10
                            columnSpacing: 18

                            Text {
                                text: "Origem"
                                color: theme.textMuted
                                font.pixelSize: 12
                                font.weight: Font.DemiBold
                            }

                            Text {
                                text: appVm.audio.streamOrigin
                                color: theme.text
                                font.pixelSize: 13
                            }

                            Text {
                                text: "Signal path"
                                color: theme.textMuted
                                font.pixelSize: 12
                                font.weight: Font.DemiBold
                            }

                            Text {
                                text: appVm.audio.signalPath
                                color: theme.text
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                            }

                            Text {
                                text: "Cache de capas"
                                color: theme.textMuted
                                font.pixelSize: 12
                                font.weight: Font.DemiBold
                            }

                            Text {
                                text: appVm.audio.cacheUsageSummary
                                color: theme.text
                                font.pixelSize: 13
                            }

                            Text {
                                text: "Faixas em cache"
                                color: theme.textMuted
                                font.pixelSize: 12
                                font.weight: Font.DemiBold
                            }

                            Text {
                                text: appVm.audio.cachedTrackCount
                                color: theme.text
                                font.pixelSize: 13
                            }

                            Text {
                                text: "Limite"
                                color: theme.textMuted
                                font.pixelSize: 12
                                font.weight: Font.DemiBold
                            }

                            RowLayout {
                                spacing: 12

                                Slider {
                                    id: cacheSlider
                                    Layout.preferredWidth: 220
                                    from: 64
                                    to: 2048
                                    stepSize: 64
                                    value: appVm.audio.streamCacheLimitMb
                                    onMoved: appVm.audio.streamCacheLimitMb = Math.round(value)
                                }

                                Text {
                                    text: appVm.audio.streamCacheLimitMb + " MB"
                                    color: theme.text
                                    font.pixelSize: 13
                                    font.weight: Font.DemiBold
                                }
                            }
                        }

                        Row {
                            width: parent.width
                            spacing: 10

                            AppButton {
                                width: 170
                                theme: root.theme
                                text: "Limpar cache"
                                onClicked: appVm.audio.clearCoverCache()
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
            }

            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AsNeeded
            }
        }
    }
}
