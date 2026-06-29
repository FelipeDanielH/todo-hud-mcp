import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls
import FocusHUD

Window {
    id: win

    required property AppController app

    width: 348
    visible: true
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    color: "transparent"

    property bool compactMode: false
    property bool showHistory: false
    height: win.compactMode ? 64 : 500

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

            // --- Active tasks card (fills remaining space) ---
            HudCard {
                id: activeCard
                Layout.fillWidth: true
                Layout.fillHeight: true
                visible: !win.compactMode && !win.showHistory
                header: "Tareas"
                badge: {
                    if (!win.app.online) return "Offline";
                    if (win.app.wsState === 0) return "";
                    if (win.app.wsState === 2) return "";
                    if (win.app.wsState === 3) return "Reconnecting…";
                    return "";
                }

                ListView {
                    id: activeList
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    spacing: 6
                    model: win.app.taskListModel

                    delegate: Item {
                        readonly property bool isSection: model.status === "section-completed"
                        implicitHeight: isSection ? 30 : (model.phaseName && model.phaseName.length > 0 && model.status !== "completed" ? 42 : 32)
                        width: ListView.view.width

                        TaskItem {
                            visible: !isSection
                            anchors.fill: parent
                            taskId: model.taskId
                            title: model.title
                            completed: model.completed
                            status: model.status
                            phaseName: model.phaseName ?? ""
                            sortOrder: model.sortOrder ?? 0
                            onToggleComplete: function(tid) {
                                if (model.completed)
                                    win.app.reopenTask(tid)
                                else
                                    win.app.completeTask(tid)
                            }
                        }

                        Rectangle {
                            visible: isSection
                            anchors.fill: parent
                            color: "transparent"
                            Text {
                                anchors { left: parent.left; verticalCenter: parent.verticalCenter }
                                text: model.title ?? ""
                                color: Theme.dimText
                                font { pixelSize: 11; weight: Font.Bold; letterSpacing: 1; family: Theme.fontFamily }
                            }
                        }
                    }
                }
            }

            // --- Add task input ---
            Rectangle {
                id: inputRow
                Layout.fillWidth: true
                Layout.preferredHeight: 36
                visible: !win.compactMode && !win.showHistory
                color: Theme.card
                radius: 8
                border { color: Theme.border; width: 1 }

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 10
                    anchors.rightMargin: 4
                    anchors.topMargin: 2
                    anchors.bottomMargin: 2
                    spacing: 6

                    TextField {
                        id: taskInput
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter
                        color: Theme.text
                        font { pixelSize: 12; family: Theme.fontFamily }
                        placeholderText: "Add a task..."
                        verticalAlignment: TextInput.AlignVCenter
                        background: null
                        padding: 0
                        onAccepted: {
                            if (text.trim().length > 0) {
                                win.app.createTask(text.trim())
                                text = ""
                            }
                        }
                    }

                    Rectangle {
                        Layout.preferredWidth: 28
                        Layout.preferredHeight: 28
                        radius: 6
                        color: mouseAdd.containsMouse ? Theme.accentPressed : Theme.accent
                        Layout.alignment: Qt.AlignVCenter
                        Text {
                            anchors.centerIn: parent
                            text: "+"
                            color: Theme.bg
                            font { pixelSize: 18; weight: Font.Bold; family: Theme.fontFamily }
                        }
                        MouseArea {
                            id: mouseAdd
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            hoverEnabled: true
                            onClicked: {
                                if (taskInput.text.trim().length > 0) {
                                    win.app.createTask(taskInput.text.trim())
                                    taskInput.text = ""
                                }
                            }
                        }
                    }
                }
            }

            // --- Action buttons ---
            RowLayout {
                Layout.fillWidth: true
                visible: !win.compactMode && !win.showHistory
                spacing: 6

                Rectangle {
                    Layout.preferredWidth: 32
                    Layout.preferredHeight: 32
                    radius: 6
                    color: mouseRefresh.containsMouse ? Theme.accentPressed : Theme.accent
                    visible: win.app.wsState !== 2 && win.app.online
                    Text {
                        anchors.centerIn: parent
                        text: "↻"
                        color: Theme.bg
                        font { pixelSize: 14; weight: Font.Bold; family: Theme.fontFamily }
                    }
                    MouseArea {
                        id: mouseRefresh
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        hoverEnabled: true
                        onClicked: win.app.forceRefresh()
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 32
                    radius: 6
                    color: mouseArchive.containsMouse ? Theme.accentPressed : Theme.accent
                    visible: win.app.completedCount > 0
                    Text {
                        anchors.centerIn: parent
                        text: "Archive completed (%1)".arg(win.app.completedCount)
                        color: Theme.bg
                        font { pixelSize: 11; weight: Font.Bold; family: Theme.fontFamily }
                    }
                    MouseArea {
                        id: mouseArchive
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        hoverEnabled: true
                        onClicked: win.app.archiveCompleted()
                    }
                }

                Rectangle {
                    Layout.preferredWidth: 60
                    Layout.preferredHeight: 32
                    radius: 6
                    color: mouseHistoryBtn.containsMouse ? Theme.accentPressed : Theme.accent
                    Text {
                        anchors.centerIn: parent
                        text: "History"
                        color: Theme.bg
                        font { pixelSize: 11; weight: Font.Bold; family: Theme.fontFamily }
                    }
                    MouseArea {
                        id: mouseHistoryBtn
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        hoverEnabled: true
                        onClicked: win.showHistory = !win.showHistory
                    }
                }
            }

            // --- Compact mode bar ---
            Rectangle {
                id: compactBar
                visible: win.compactMode
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "transparent"
                Text {
                    anchors { left: parent.left; leftMargin: 4; verticalCenter: parent.verticalCenter }
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

            // --- History panel ---
            HistoryPanel {
                visible: win.showHistory && !win.compactMode
                Layout.fillWidth: true
                Layout.fillHeight: true
                onClose: win.showHistory = false
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
