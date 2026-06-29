import QtQuick

Rectangle {
    id: root

    property alias header: headerText.text
    property alias badge: badgeText.text
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
                id: headerText
                color: Theme.text
                font { pixelSize: 16; weight: Font.Bold; family: Theme.family }
                anchors.verticalCenter: parent.verticalCenter
            }

            Rectangle {
                id: badgeRect
                height: 20
                width: badgeText.width + 12
                radius: 4
                color: Theme.badge
                anchors.verticalCenter: parent.verticalCenter
                visible: badgeText.text.length > 0

                Text {
                    id: badgeText
                    anchors.centerIn: parent
                    color: "#0f0f1a"
                    font { pixelSize: 10; weight: Font.Bold; family: Theme.family }
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
