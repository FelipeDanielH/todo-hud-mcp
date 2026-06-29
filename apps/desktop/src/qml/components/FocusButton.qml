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
        color: root.down ? Theme.accentPressed : Theme.accent
    }

    contentItem: Text {
        text: root.label
        color: Theme.text
        font { pixelSize: 14; weight: Font.Bold; family: Theme.fontFamily }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    Behavior on scale {
        NumberAnimation { duration: 80 }
    }
}
