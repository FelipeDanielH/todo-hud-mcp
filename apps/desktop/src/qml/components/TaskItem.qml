import QtQuick
import FocusHUD

Rectangle {
    id: root

    required property int taskId
    required property string title
    required property bool completed
    required property string status
    property string phaseName: ""
    property int sortOrder: 0

    signal selected(int taskId)
    signal toggleComplete(int taskId)

    readonly property bool done: status === "completed" || completed
    readonly property string tagText: phaseName.length > 0 ? phaseName : "General"
    readonly property bool purpleTag: tagText.toLowerCase().indexOf("general") >= 0

    implicitHeight: 55
    radius: 10
    color: taskMouse.containsMouse ? Theme.cardHover : Theme.card
    border { color: Theme.borderSoft; width: 1 }

    IconGlyph {
        id: statusCircle
        z: 2
        x: 13
        anchors.verticalCenter: parent.verticalCenter
        width: 22
        height: 22
        glyph: root.done ? "check-filled" : "circle"
        strokeColor: root.done ? Theme.bg : Theme.dimText
        fillColor: root.done ? Theme.success : "transparent"
        lineWidth: root.done ? 2 : 1.6

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: root.toggleComplete(root.taskId)
        }
    }

    Column {
        id: copy
        x: 43
        anchors.verticalCenter: parent.verticalCenter
        width: Math.max(80, parent.width - 74)
        spacing: root.done ? 8 : 5

        Text {
            width: parent.width
            text: root.title
            color: root.done ? Theme.dimText : Theme.text
            opacity: root.done ? 0.78 : 1
            elide: Text.ElideRight
            font {
                pixelSize: 13
                weight: root.done ? Font.Normal : Font.DemiBold
                family: Theme.fontFamily
                strikeout: root.done
            }
        }

        Item {
            width: tagLabel.width
            height: root.done && !root.purpleTag ? 13 : 18

            Rectangle {
                anchors.fill: parent
                radius: 4
                color: root.purpleTag ? Theme.purpleBg : Theme.blueBg
                opacity: root.done && !root.purpleTag ? 0 : 0.95
            }

            Text {
                id: tagLabel
                leftPadding: root.done && !root.purpleTag ? 0 : 6
                rightPadding: root.done && !root.purpleTag ? 0 : 6
                text: root.tagText
                color: root.done && !root.purpleTag ? Theme.dimText : (root.purpleTag ? Theme.purple : Theme.blue)
                elide: Text.ElideRight
                font { pixelSize: 10; family: Theme.fontFamily }
            }
        }
    }

    IconGlyph {
        id: menuDots
        anchors {
            right: parent.right
            rightMargin: 13
            verticalCenter: parent.verticalCenter
        }
        width: 20
        height: 20
        glyph: "dots"
        strokeColor: Theme.dimText
        fillColor: "transparent"
        lineWidth: 1.4
    }

    MouseArea {
        id: taskMouse
        z: 1
        anchors.fill: parent
        anchors.leftMargin: 43
        acceptedButtons: Qt.LeftButton
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: root.selected(root.taskId)
    }

    MouseArea {
        z: 10
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
        width: 43
        acceptedButtons: Qt.LeftButton
        cursorShape: Qt.PointingHandCursor
        onClicked: root.toggleComplete(root.taskId)
    }

    component IconGlyph: Canvas {
        id: icon
        property string glyph: "circle"
        property color strokeColor: Theme.text
        property color fillColor: "transparent"
        property real lineWidth: 1.8

        onGlyphChanged: requestPaint()
        onStrokeColorChanged: requestPaint()
        onFillColorChanged: requestPaint()
        onLineWidthChanged: requestPaint()
        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)
            ctx.lineCap = "round"
            ctx.lineJoin = "round"
            ctx.lineWidth = lineWidth
            ctx.strokeStyle = strokeColor
            ctx.fillStyle = fillColor

            if (glyph === "circle") {
                ctx.beginPath()
                ctx.arc(width / 2, height / 2, Math.min(width, height) / 2 - lineWidth, 0, Math.PI * 2)
                ctx.stroke()
            } else if (glyph === "check-filled") {
                ctx.beginPath()
                ctx.arc(width / 2, height / 2, Math.min(width, height) / 2 - 1, 0, Math.PI * 2)
                ctx.fill()
                ctx.strokeStyle = strokeColor
                ctx.lineWidth = 2.2
                ctx.beginPath()
                ctx.moveTo(width * 0.32, height * 0.53)
                ctx.lineTo(width * 0.45, height * 0.65)
                ctx.lineTo(width * 0.70, height * 0.36)
                ctx.stroke()
            } else if (glyph === "check-ring") {
                ctx.beginPath()
                ctx.arc(width / 2, height / 2, Math.min(width, height) / 2 - lineWidth, 0, Math.PI * 2)
                ctx.stroke()
                ctx.beginPath()
                ctx.moveTo(width * 0.33, height * 0.53)
                ctx.lineTo(width * 0.45, height * 0.65)
                ctx.lineTo(width * 0.70, height * 0.36)
                ctx.stroke()
            } else if (glyph === "calendar") {
                ctx.strokeRect(width * 0.18, height * 0.25, width * 0.64, height * 0.58)
                ctx.beginPath()
                ctx.moveTo(width * 0.18, height * 0.42)
                ctx.lineTo(width * 0.82, height * 0.42)
                ctx.moveTo(width * 0.34, height * 0.17)
                ctx.lineTo(width * 0.34, height * 0.31)
                ctx.moveTo(width * 0.66, height * 0.17)
                ctx.lineTo(width * 0.66, height * 0.31)
                ctx.stroke()
            } else if (glyph === "clock") {
                ctx.beginPath()
                ctx.arc(width / 2, height / 2, Math.min(width, height) / 2 - lineWidth, 0, Math.PI * 2)
                ctx.stroke()
                ctx.beginPath()
                ctx.moveTo(width / 2, height / 2)
                ctx.lineTo(width / 2, height * 0.30)
                ctx.moveTo(width / 2, height / 2)
                ctx.lineTo(width * 0.66, height * 0.58)
                ctx.stroke()
            } else if (glyph === "dots") {
                ctx.fillStyle = strokeColor
                for (var i = 0; i < 3; ++i) {
                    ctx.beginPath()
                    ctx.arc(width * (0.32 + i * 0.18), height / 2, 1.45, 0, Math.PI * 2)
                    ctx.fill()
                }
            }
        }
    }
}
