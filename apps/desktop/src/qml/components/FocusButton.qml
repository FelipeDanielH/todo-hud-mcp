import QtQuick
import QtQuick.Controls
import FocusHUD

Button {
    id: root

    required property string label

    width: parent ? parent.width : 0
    height: 44

    background: Rectangle {
        radius: Theme.radius
        color: root.down ? "#00b898" : (root.hovered ? "#00e6b3" : Theme.accent)
    }

    contentItem: Text {
        text: root.label
        anchors.centerIn: parent
        color: "#0f0f1a"
        font { pixelSize: 14; weight: Font.Bold; family: Theme.fontFamily }
        horizontalAlignment: Text.AlignHCenter
    }

    Behavior on scale {
        NumberAnimation { duration: 80 }
    }
}
