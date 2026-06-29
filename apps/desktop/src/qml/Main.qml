pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import FocusHUD

Window {
    id: win

    required property AppController app

    width: 348
    height: 500
    visible: true
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    color: "transparent"

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

            // HudCard {
            //     id: activeCard
            //     Layout.fillWidth: true
            //     Layout.preferredHeight: 210
            //     header: "Focus HUD"
            //     badge: "MCP"
            //
            //     Text {
            //         Layout.fillWidth: true
            //         text: "Ahora"
            //         color: Theme.dimText
            //         font { pixelSize: 11; letterSpacing: 2; family: Theme.fontFamily }
            //     }
            //
            //     Text {
            //         id: activeTaskLabel
            //         Layout.fillWidth: true
            //         text: win.app.hasActiveTask
            //               ? win.app.currentTaskTitle
            //               : "Sin tarea activa"
            //         color: win.app.hasActiveTask ? Theme.text : Theme.dimText
            //         font { pixelSize: 15; weight: Font.DemiBold; family: Theme.fontFamily }
            //         elide: Text.ElideRight
            //     }
            //
            //     Text {
            //         id: timerDisplay
            //         Layout.fillWidth: true
            //         horizontalAlignment: Text.AlignHCenter
            //         text: win.app.focusTimer.formattedTime
            //         color: Theme.text
            //         font { pixelSize: 46; weight: Font.Light; family: Theme.fontFamily }
            //     }
            //
            //     FocusButton {
            //         id: focusBtn
            //         Layout.fillWidth: true
            //         label: win.app.focusTimer.isRunning ? "Detener foco" : "Iniciar foco"
            //         onClicked: {
            //             if (win.app.focusTimer.isRunning)
            //                 win.app.focusTimer.stop()
            //             else
            //                 win.app.focusTimer.start()
            //         }
            //     }
            // }

            HudCard {
                id: listCard
                Layout.fillWidth: true
                Layout.fillHeight: true
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
        }

        MouseArea {
            id: dragArea
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
                label: "-"
                onClicked: win.showMinimized()
            }

            WindowControl {
                label: "X"
                hoverColor: Theme.pending
                onClicked: win.close()
            }
        }
    }
}
