import QtQuick
import QtQuick.Controls

Button {
    id: root

    property alias label: btnText.text

    width: parent.width
    height: 44
    radius: Theme.radius

    background: Rectangle {
        radius: Theme.radius
        color: root.down ? "#00b898" : (root.hovered ? "#00e6b3" : Theme.accent)
    }

    contentItem: Text {
        id: btnText
        anchors.centerIn: parent
        color: "#0f0f1a"
        font { pixelSize: 14; weight: Font.Bold; family: Theme.family }
        horizontalAlignment: Text.AlignHCenter
    }

    Behavior on scale {
        NumberAnimation { duration: 80 }
    }
}
