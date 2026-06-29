import QtQuick

Row {
    id: root

    property alias taskTitle: label.text
    property bool completed: false

    spacing: 10

    Rectangle {
        width: 10; height: 10
        radius: 5
        anchors.verticalCenter: parent.verticalCenter
        color: root.completed ? Theme.success : Theme.pending
    }

    Text {
        id: label
        color: root.completed ? Theme.dimText : Theme.text
        font { pixelSize: 13; family: Theme.family }
        elide: Text.ElideRight
        width: parent.parent.width - 20
        opacity: root.completed ? 0.6 : 1.0
    }
}
