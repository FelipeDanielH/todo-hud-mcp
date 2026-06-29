import QtQuick
import QtQuick.Layouts
import FocusHUD

Item {
    id: root

    required property int taskId
    required property string title
    required property bool completed
    required property string status
    property string phaseName: ""
    property int sortOrder: 0

    signal selected(int taskId)
    signal toggleComplete(int taskId)

    implicitHeight: phaseName.length > 0 ? 42 : 32

    RowLayout {
        anchors.fill: parent
        spacing: 10

        Rectangle {
            Layout.alignment: Qt.AlignVCenter
            Layout.preferredWidth: 18
            Layout.preferredHeight: 18
            radius: 9
            border {
                color: root.status === "completed" ? Theme.success : Theme.dimText
                width: 2
            }
            color: root.status === "completed" ? Theme.success : "transparent"

            Text {
                anchors.centerIn: parent
                text: "✓"
                color: Theme.bg
                font { pixelSize: 11; weight: Font.Bold; family: Theme.fontFamily }
                visible: root.status === "completed"
            }

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: root.toggleComplete(root.taskId)
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            spacing: 0

            Text {
                Layout.fillWidth: true
                text: root.title
                color: root.status === "completed" ? Theme.dimText : Theme.text
                font { pixelSize: 13; family: Theme.fontFamily; strikeout: root.status === "completed" }
                elide: Text.ElideRight
                opacity: root.status === "completed" ? 0.6 : 1.0
            }

            Text {
                Layout.fillWidth: true
                text: root.phaseName
                color: Theme.dimText
                font { pixelSize: 10; family: Theme.fontFamily }
                elide: Text.ElideRight
                visible: root.phaseName.length > 0 && root.status !== "completed"
            }
        }
    }
}
