import QtQuick
import FocusHUD

Rectangle {
    id: root

    required property string header
    property string badge: ""
    default property alias content: contentArea.data

    color: Theme.card
    radius: Theme.radius
    border { color: Theme.border; width: 1 }

    Column {
        anchors.fill: parent
        anchors.margins: Theme.padding
        spacing: Theme.spacing

        Row {
            width: parent.width
            spacing: 10

            Text {
                text: root.header
                color: Theme.text
                font { pixelSize: 16; weight: Font.Bold; family: Theme.fontFamily }
                anchors.verticalCenter: parent.verticalCenter
            }

            Rectangle {
                id: badgeRect
                height: 20
                width: badgeLabel.width + 12
                radius: 4
                color: Theme.badge
                anchors.verticalCenter: parent.verticalCenter
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

        Item { width: 1; height: 4 }

        Column {
            id: contentArea
            width: parent.width
            spacing: Theme.spacing
        }
    }
}
