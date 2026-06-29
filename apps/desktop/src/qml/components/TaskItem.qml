import QtQuick
import QtQuick.Layouts
import FocusHUD

Item {
    id: root

    required property int taskId
    required property string title
    required property bool completed

    signal selected(int taskId)

    implicitHeight: 32
    height: implicitHeight

    RowLayout {
        anchors.fill: parent
        spacing: 10

        Rectangle {
            Layout.alignment: Qt.AlignVCenter
            Layout.preferredWidth: 10
            Layout.preferredHeight: 10
            radius: 5
            color: root.completed ? Theme.success : Theme.pending
        }

        Text {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            text: root.title
            color: root.completed ? Theme.dimText : Theme.text
            font { pixelSize: 13; family: Theme.fontFamily }
            elide: Text.ElideRight
            opacity: root.completed ? 0.6 : 1.0
        }
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: root.selected(root.taskId)
    }
}
