import QtQuick
import QtQuick.Controls

ComboBox {
    id: control

    property color accentColor: "#d99f5d"
    property color textColor: "#f5f7fb"

    implicitHeight: 44
    font.pixelSize: 14

    delegate: ItemDelegate {
        width: control.width
        height: 40
        highlighted: control.highlightedIndex === index

        background: Rectangle {
            color: parent.highlighted
                   ? Qt.rgba(control.accentColor.r, control.accentColor.g, control.accentColor.b, 0.20)
                   : "transparent"
        }

        contentItem: Text {
            text: modelData
            color: control.textColor
            font.pixelSize: 14
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
    }

    contentItem: Text {
        leftPadding: 16
        rightPadding: 36
        text: control.displayText
        color: control.textColor
        font: control.font
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    indicator: Canvas {
        x: control.width - width - 14
        y: control.topPadding + (control.availableHeight - height) / 2
        width: 10
        height: 6
        contextType: "2d"

        onPaint: {
            context.reset()
            context.moveTo(0, 0)
            context.lineTo(width, 0)
            context.lineTo(width / 2, height)
            context.closePath()
            context.fillStyle = "#d7dbe4"
            context.fill()
        }
    }

    background: Rectangle {
        radius: 14
        color: Qt.rgba(0.05, 0.06, 0.09, control.popup.visible ? 0.88 : 0.72)
        border.width: 1
        border.color: control.popup.visible
                      ? Qt.rgba(control.accentColor.r, control.accentColor.g, control.accentColor.b, 0.68)
                      : Qt.rgba(1, 1, 1, 0.09)
    }

    popup: Popup {
        y: control.height + 6
        width: control.width
        implicitHeight: contentItem.implicitHeight + 12
        padding: 6

        background: Rectangle {
            radius: 16
            color: Qt.rgba(0.06, 0.07, 0.10, 0.98)
            border.width: 1
            border.color: Qt.rgba(1, 1, 1, 0.08)
        }

        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex
            ScrollIndicator.vertical: ScrollIndicator { }
        }
    }
}
