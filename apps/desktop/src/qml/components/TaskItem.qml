import QtQuick
import FocusHUD

Row {
    id: root

    required property string title
    property bool completed: false

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
}
