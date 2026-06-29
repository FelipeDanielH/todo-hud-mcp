import QtQuick
import QtQuick.Layouts
import FocusHUD

Rectangle {
    id: root

    required property string header
    property string badge: ""
    default property alias content: contentArea.data

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
                text: root.header
                color: Theme.text
                font { pixelSize: 16; weight: Font.Bold; family: Theme.fontFamily }
                elide: Text.ElideRight
            }

            Rectangle {
                id: badgeRect
                Layout.alignment: Qt.AlignVCenter
                implicitHeight: 20
                implicitWidth: badgeLabel.implicitWidth + 12
                radius: 4
                color: Theme.badge
                visible: root.badge.length > 0

                Text {
                    id: badgeLabel
                    anchors.centerIn: parent
                    color: "#0f0f1a"
                    font { pixelSize: 10; weight: Font.Bold; family: Theme.fontFamily }
                    text: root.badge
                }
            }
        }

        ColumnLayout {
            id: contentArea
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: Theme.spacing
        }
    }
}
