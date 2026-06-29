pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import FocusHUD

Window {
    id: win

    required property AppController app

    width: 348
    visible: true
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    color: "transparent"

    property bool compactMode: false
    height: 500

    Behavior on height { NumberAnimation { duration: 200; easing.type: Easing.InOutQuad } }
    Behavior on y { NumberAnimation { duration: 200; easing.type: Easing.InOutQuad } }

    onCompactModeChanged: {
        if (win.compactMode) {
            win.y += win.height - 64
            win.height = 64
        } else {
            win.height = 500
            win.y -= 500 - 64
        }
    }

    component WindowControl: Rectangle {
        id: control

        required property string label
        property color hoverColor: Theme.border
        signal clicked()

        Layout.preferredWidth: 26
        Layout.preferredHeight: 24
        radius: 6
        color: mouse.containsMouse ? control.hoverColor : "transparent"

        Text {
            anchors.centerIn: parent
            text: control.label
            color: Theme.text
            font { pixelSize: 13; weight: Font.Bold; family: Theme.fontFamily }
        }

        MouseArea {
            id: mouse
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: control.clicked()
        }
    }

    Rectangle {
        anchors.fill: parent
        anchors.margins: 8
        color: Theme.bg
        radius: Theme.radius
        border { color: Theme.border; width: 1 }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: Theme.padding
            spacing: Theme.spacing

            HudCard {
                id: listCard
                Layout.fillWidth: true
                Layout.fillHeight: true
                visible: !win.compactMode
                header: "Tareas"
                badge: ""

                ListView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    spacing: Theme.spacing
                    model: win.app.taskListModel

                    delegate: TaskItem {
                        width: ListView.view.width
                        onSelected: function(taskId) { win.app.selectTask(taskId) }
                    }
                }
            }

            Rectangle {
                id: compactBar
                visible: win.compactMode
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "transparent"

                Text {
                    anchors {
                        left: parent.left; leftMargin: 4
                        verticalCenter: parent.verticalCenter
                    }
                    text: win.app.hasActiveTask ? win.app.currentTaskTitle : "Focus HUD"
                    color: Theme.text
                    font { pixelSize: 15; weight: Font.DemiBold; family: Theme.fontFamily }
                    elide: Text.ElideRight
                    width: parent.width - 8
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    acceptedButtons: Qt.LeftButton
                    onClicked: win.compactMode = false
                    onPressed: win.startSystemMove()
                }
            }
        }

        MouseArea {
            id: dragArea
            enabled: !win.compactMode
            height: 42
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                leftMargin: 10
                rightMargin: 76
                topMargin: 8
            }
            acceptedButtons: Qt.LeftButton
            cursorShape: Qt.SizeAllCursor
            onPressed: win.startSystemMove()
        }

        RowLayout {
            id: windowControls
            height: 24
            spacing: 4
            anchors {
                top: parent.top
                right: parent.right
                topMargin: 10
                rightMargin: 10
            }

            WindowControl {
                label: win.compactMode ? "+" : "-"
                onClicked: win.compactMode = !win.compactMode
            }

            WindowControl {
                label: "X"
                hoverColor: Theme.pending
                onClicked: win.close()
            }
        }
    }
}
