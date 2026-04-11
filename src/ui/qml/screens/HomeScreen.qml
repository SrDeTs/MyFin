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

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 18
            spacing: 14

            RowLayout {
                Layout.fillWidth: true
                spacing: 14

                TextField {
                    Layout.fillWidth: true
                    implicitHeight: 42
                    text: appVm.library.searchQuery
                    placeholderText: "Pesquisar musicas"
                    color: theme.text
                    placeholderTextColor: theme.textMuted
                    leftPadding: 14
                    rightPadding: 14
                    topPadding: 0
                    bottomPadding: 0
                    verticalAlignment: TextInput.AlignVCenter
                    selectByMouse: true
                    onTextEdited: appVm.library.searchQuery = text

                    background: Rectangle {
                        radius: 14
                        color: theme.panelRaised
                        border.width: 1
                        border.color: parent.activeFocus ? theme.accentStrong : theme.stroke
                    }
                }

                Rectangle {
                    radius: 12
                    color: theme.panelRaised
                    border.width: 1
                    border.color: theme.stroke
                    implicitHeight: 34
                    implicitWidth: 120

                    Text {
                        anchors.centerIn: parent
                        text: appVm.library.trackCount + " faixas"
                        color: theme.text
                        font.pixelSize: 12
                        font.weight: Font.DemiBold
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                ListView {
                    id: tracksView
                    anchors.fill: parent
                    clip: true
                    spacing: 8
                    reuseItems: true
                    boundsBehavior: Flickable.StopAtBounds
                    visible: appVm.library.hasTracks
                    model: appVm.library.tracks

                    ScrollBar.vertical: ScrollBar {
                        policy: ScrollBar.AsNeeded
                    }

                    delegate: Rectangle {
                        required property string trackId
                        required property string title
                        required property string artist
                        required property string album
                        required property string durationText
                        required property string coverSource
                        required property string signalPath
                        required property bool favorite

                        width: tracksView.width
                        height: 70
                        radius: 16
                        color: mouseArea.containsMouse ? theme.panelRaised : theme.panelMuted
                        border.width: 1
                        border.color: mouseArea.containsMouse ? theme.accent : theme.stroke

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 12
                            spacing: 12

                            Rectangle {
                                Layout.preferredWidth: 44
                                Layout.preferredHeight: 44
                                radius: 12
                                color: theme.panelRaised
                                border.width: 1
                                border.color: theme.stroke

                                Image {
                                    anchors.fill: parent
                                    anchors.margins: 1
                                    asynchronous: true
                                    cache: false
                                    fillMode: Image.PreserveAspectCrop
                                    sourceSize.width: 44
                                    sourceSize.height: 44
                                    source: coverSource.length > 0 ? coverSource : ""
                                    visible: coverSource.length > 0
                                }

                                Text {
                                    anchors.centerIn: parent
                                    visible: coverSource.length === 0
                                    text: "MF"
                                    color: theme.textMuted
                                    font.pixelSize: 11
                                    font.weight: Font.Bold
                                }
                            }

                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 1

                                Text {
                                    Layout.fillWidth: true
                                    text: title
                                    color: theme.text
                                    font.pixelSize: 15
                                    font.weight: Font.DemiBold
                                    elide: Text.ElideRight
                                }

                                Text {
                                    Layout.fillWidth: true
                                    text: artist + " • " + album
                                    color: theme.textMuted
                                    font.pixelSize: 13
                                    elide: Text.ElideRight
                                }
                            }

                            Rectangle {
                                radius: 10
                                color: favorite ? theme.accent : theme.panelRaised
                                border.width: 1
                                border.color: favorite ? theme.accentStrong : theme.stroke
                                implicitHeight: 24
                                implicitWidth: favorite ? 76 : 88

                                Text {
                                    anchors.centerIn: parent
                                    text: favorite ? "Favorita" : signalPath
                                    color: favorite ? theme.window : theme.textMuted
                                    font.pixelSize: 11
                                    font.weight: Font.DemiBold
                                }
                            }

                            Text {
                                text: durationText
                                color: theme.textMuted
                                font.pixelSize: 13
                                font.weight: Font.DemiBold
                            }
                        }

                        MouseArea {
                            id: mouseArea
                            anchors.fill: parent
                            hoverEnabled: true
                            enabled: appVm.session.authenticated
                            onClicked: appVm.library.playTrack(trackId)
                        }
                    }
                }

                Rectangle {
                    anchors.fill: parent
                    visible: !appVm.library.hasTracks
                    radius: 18
                    color: theme.panelMuted
                    border.width: 1
                    border.color: theme.stroke

                    Column {
                        anchors.centerIn: parent
                        width: 460
                        spacing: 12

                        Text {
                            width: parent.width
                            text: appVm.library.searchQuery.length > 0
                                  ? "Nenhum resultado encontrado"
                                  : appVm.session.authenticated
                                    ? "Nenhuma musica carregada"
                                    : "Nenhuma biblioteca local disponivel"
                            color: theme.text
                            font.pixelSize: 24
                            font.weight: Font.Bold
                            horizontalAlignment: Text.AlignHCenter
                            wrapMode: Text.WordWrap
                        }

                        Text {
                            width: parent.width
                            text: appVm.library.searchQuery.length > 0
                                  ? "Tente outro termo de busca."
                                  : appVm.session.authenticated
                                    ? "Atualize a biblioteca ou verifique se sua conta no Jellyfin tem permissao para acessar itens de audio."
                                    : "Abra Configuracoes para conectar no Jellyfin e criar o primeiro snapshot local."
                            color: theme.textMuted
                            font.pixelSize: 14
                            horizontalAlignment: Text.AlignHCenter
                            wrapMode: Text.WordWrap
                        }
                    }
                }
            }
        }
    }
}
