import QtQuick
import FocusHUD

Rectangle {
    id: root

    signal close()

    color: Theme.panelDeep
    radius: 10
    border { color: Theme.borderSoft; width: 1 }

    Text {
        x: 20
        y: 18
        text: "Historial"
        color: Theme.text
        font { pixelSize: 17; weight: Font.Bold; family: Theme.fontFamily }
    }

    Rectangle {
        anchors {
            top: parent.top
            right: parent.right
            topMargin: 13
            rightMargin: 13
        }
        width: 32
        height: 32
        radius: 8
        color: closeMouse.containsMouse ? Theme.cardHover : Theme.panel
        border { color: Theme.borderSoft; width: 1 }

        Text {
            anchors.centerIn: parent
            text: "<"
            color: Theme.text
            font { pixelSize: 15; weight: Font.Bold; family: Theme.fontFamily }
        }

        MouseArea {
            id: closeMouse
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            hoverEnabled: true
            onClicked: root.close()
        }
    }

    Rectangle {
        x: 20
        y: 59
        width: parent.width - 40
        height: 1
        color: Theme.borderSoft
    }

    Text {
        anchors.centerIn: parent
        text: "No hay tareas archivadas todavía."
        color: Theme.mutedText
        font { pixelSize: 12; family: Theme.fontFamily }
    }
}
