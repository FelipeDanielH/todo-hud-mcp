import QtQuick
import FocusHUD

Row {
    id: root

    required property int taskId
    required property string title
    required property bool completed

    signal selected(int taskId)

    spacing: 10
    height: 28

    Rectangle {
        width: 10; height: 10
        radius: 5
        anchors.verticalCenter: parent.verticalCenter
        color: root.completed ? Theme.success : Theme.pending
    }

    Text {
        text: root.title
        color: root.completed ? Theme.dimText : Theme.text
        font { pixelSize: 13; family: Theme.fontFamily }
        elide: Text.ElideRight
        width: parent.parent.width - 20
        opacity: root.completed ? 0.6 : 1.0
        anchors.verticalCenter: parent.verticalCenter
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: root.selected(root.taskId)
    }
}
