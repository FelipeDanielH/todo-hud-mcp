import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import "components"
import "theme"

Window {
    id: win
    width: 360
    height: 520
    visible: true
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    color: "transparent"

    Rectangle {
        anchors.fill: parent
        anchors.margins: 8
        color: Theme.bg
        radius: Theme.radius

        Column {
            anchors.fill: parent
            anchors.margins: Theme.padding
            spacing: Theme.spacing

            HudCard {
                id: activeCard
                width: parent.width
                height: 180
                header: "Focus HUD"
                badge: "MCP"

                Text {
                    width: parent.width
                    text: "Ahora"
                    color: Theme.dimText
                    font { pixelSize: 11; letterSpacing: 2; family: Theme.family }
                }

                Text {
                    id: activeTaskLabel
                    width: parent.width
                    text: taskController.hasActiveTask
                          ? taskController.currentTaskTitle
                          : "Sin tarea activa"
                    color: taskController.hasActiveTask ? Theme.text : Theme.dimText
                    font { pixelSize: 15; weight: Font.DemiBold; family: Theme.family }
                    elide: Text.ElideRight
                }

                Text {
                    id: timerDisplay
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    text: taskController.formattedTime.length > 0
                          ? taskController.formattedTime
                          : "25:00"
                    color: Theme.text
                    font { pixelSize: 42; weight: Font.Light; family: Theme.family }
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                FocusButton {
                    id: focusBtn
                    label: taskController.isRunning ? "Detener foco" : "Iniciar foco"
                    onClicked: {
                        if (taskController.isRunning)
                            taskController.stopFocus()
                        else
                            taskController.startFocus()
                    }
                }
            }

            Item { width: 1; height: 6 }

            HudCard {
                id: listCard
                width: parent.width
                height: win.height - activeCard.height - Theme.padding * 5 - 8
                header: "Tareas"
                badge: ""

                Repeater {
                    model: taskController.taskModel

                    delegate: TaskItem {
                        width: parent.width
                        height: 28
                        taskTitle: model.title
                        completed: model.status === "completada"

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: taskController.selectTask(index)
                        }
                    }
                }
            }
        }

        Rectangle {
            id: closeBtn
            width: 12; height: 12
            radius: 6
            color: "#ff6b6b"
            anchors { top: parent.top; right: parent.right; margins: 10 }

            MouseArea {
                anchors.fill: parent
                onClicked: Qt.quit()
            }
        }
    }
}
