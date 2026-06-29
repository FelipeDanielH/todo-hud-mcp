import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import FocusHUD

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

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: Theme.padding
            spacing: Theme.spacing

            HudCard {
                id: activeCard
                Layout.fillWidth: true
                Layout.preferredHeight: 220
                header: "Focus HUD"
                badge: "MCP"

                Text {
                    Layout.fillWidth: true
                    text: "Ahora"
                    color: Theme.dimText
                    font { pixelSize: 11; letterSpacing: 2; family: Theme.fontFamily }
                }

                Text {
                    id: activeTaskLabel
                    Layout.fillWidth: true
                    text: app.hasActiveTask
                          ? app.currentTaskTitle
                          : "Sin tarea activa"
                    color: app.hasActiveTask ? Theme.text : Theme.dimText
                    font { pixelSize: 15; weight: Font.DemiBold; family: Theme.fontFamily }
                    elide: Text.ElideRight
                }

                Text {
                    id: timerDisplay
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    text: app.focusTimer.formattedTime
                    color: Theme.text
                    font { pixelSize: 42; weight: Font.Light; family: Theme.fontFamily }
                }

                FocusButton {
                    id: focusBtn
                    Layout.fillWidth: true
                    label: app.focusTimer.isRunning ? "Detener foco" : "Iniciar foco"
                    onClicked: {
                        if (app.focusTimer.isRunning)
                            app.focusTimer.stop()
                        else
                            app.focusTimer.start()
                    }
                }
            }

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
                    model: app.taskListModel

                    delegate: TaskItem {
                        width: ListView.view.width
                        onSelected: function(taskId) { app.selectTask(taskId) }
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
    }
}
