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
                            text: "Saida de audio"
                            color: theme.text
                            font.pixelSize: 18
                            font.weight: Font.DemiBold
                        }

                        Text {
                            width: parent.width
                            text: "Somente controles ativos no player atual aparecem aqui."
                            color: theme.textMuted
                            font.pixelSize: 12
                            wrapMode: Text.WordWrap
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
                                text: "Dispositivo atual"
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
                        }
                    }
                }

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
                            text: "Biblioteca e cache"
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
                                text: "Origem atual"
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
