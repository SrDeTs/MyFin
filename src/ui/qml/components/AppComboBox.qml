import QtQuick
import QtQuick.Controls

ComboBox {
    id: control

    required property var theme

    implicitHeight: 42
    topPadding: 0
    bottomPadding: 0
    leftPadding: 14
    rightPadding: 42

    delegate: ItemDelegate {
        required property var modelData
        required property int index

        width: control.width
        height: 42
        highlighted: control.highlightedIndex === index

        background: Rectangle {
            color: parent.highlighted ? control.theme.panelRaised : control.theme.panelMuted
        }

        contentItem: Text {
            text: typeof modelData === "object" && modelData !== null && control.textRole !== ""
                  ? modelData[control.textRole]
                  : modelData
            color: control.theme.text
            font.pixelSize: 13
            font.weight: parent.highlighted ? Font.DemiBold : Font.Medium
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
    }

    indicator: Canvas {
        x: control.width - width - 14
        y: (control.height - height) / 2
        width: 12
        height: 8
        contextType: "2d"

        Connections {
            target: control
            function onPressedChanged() { parent.requestPaint() }
            function onDownChanged() { parent.requestPaint() }
        }

        onPaint: {
            const ctx = getContext("2d")
            ctx.reset()
            ctx.lineWidth = 1.8
            ctx.lineCap = "round"
            ctx.strokeStyle = control.theme.textMuted

            ctx.beginPath()
            ctx.moveTo(1, 1)
            ctx.lineTo(width / 2, height - 1)
            ctx.lineTo(width - 1, 1)
            ctx.stroke()
        }
    }

    contentItem: Text {
        leftPadding: 0
        rightPadding: 0
        text: control.displayText
        color: control.theme.text
        font.pixelSize: 13
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        radius: 14
        color: control.theme.panelRaised
        border.width: 1
        border.color: control.visualFocus ? control.theme.accentStrong : control.theme.stroke
    }

    popup: Popup {
        y: control.height + 6
        width: control.width
        padding: 0
        modal: false

        background: Rectangle {
            radius: 14
            color: control.theme.panel
            border.width: 1
            border.color: control.theme.stroke
        }

        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: control.delegateModel
            currentIndex: control.highlightedIndex
            boundsBehavior: Flickable.StopAtBounds
            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AsNeeded
            }
        }
    }
}
