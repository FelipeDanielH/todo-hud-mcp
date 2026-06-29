import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import FocusHUD

Rectangle {
    id: root

    signal close()

    color: Theme.card
    radius: Theme.radius
    border { color: Theme.border; width: 1 }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.padding
        spacing: Theme.spacing

        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            Text {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter
                text: "Archive"
                color: Theme.text
                font { pixelSize: 15; weight: Font.Bold; family: Theme.fontFamily }
            }

            Rectangle {
                Layout.preferredWidth: 26
                Layout.preferredHeight: 24
                radius: 6
                color: mouseClose.containsMouse ? Theme.pending : "transparent"
                Layout.alignment: Qt.AlignVCenter

                Text {
                    anchors.centerIn: parent
                    text: "←"
                    color: Theme.text
                    font { pixelSize: 13; weight: Font.Bold; family: Theme.fontFamily }
                }

                MouseArea {
                    id: mouseClose
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    hoverEnabled: true
                    onClicked: root.close()
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: Theme.border
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Text {
                anchors.centerIn: parent
                text: "No archived phases yet."
                color: Theme.dimText
                font { pixelSize: 12; family: Theme.fontFamily }
                visible: true
            }
        }
    }
}
