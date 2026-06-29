pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Window
import QtQuick.Controls
import FocusHUD

Window {
    id: win

    required property AppController app

    width: 336
    visible: true
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    color: "transparent"

    readonly property int expandedHeight: 492
    readonly property int compactHeight: 76
    property bool compactMode: false
    property bool showHistory: false
    property bool completedCollapsed: false
    property real expandedY: y
    property string selectedFilter: "all"
    readonly property int completedCountValue: win.app.completedCount
    readonly property int pendingCountValue: Math.max(0, taskListView.count - win.completedCountValue - (win.completedCountValue > 0 ? 1 : 0))
    readonly property int totalCountValue: win.pendingCountValue + win.completedCountValue

    height: win.compactMode ? win.compactHeight : win.expandedHeight

    Behavior on height { NumberAnimation { duration: 200; easing.type: Easing.InOutQuad } }
    Behavior on y { NumberAnimation { duration: 200; easing.type: Easing.InOutQuad } }

    onCompactModeChanged: {
        if (win.compactMode) {
            win.expandedY = win.y
            win.y = win.expandedY + win.expandedHeight - win.compactHeight
        } else {
            win.y = win.expandedY
        }
    }

    Rectangle {
        id: shell
        anchors.fill: parent
        radius: win.compactMode ? 22 : 22
        color: Theme.outerBg
        border { color: "#0c1621"; width: 1 }
    }

    Rectangle {
        id: frame
        anchors.fill: parent
        anchors.margins: win.compactMode ? 8 : 8
        radius: win.compactMode ? 14 : 14
        color: Theme.bg
        border { color: Theme.border; width: 1 }
        clip: true

        Rectangle {
            anchors.fill: parent
            radius: parent.radius
            color: "transparent"
            border { color: "#172538"; width: 1 }
            opacity: 0.65
        }

        Item {
            id: expandedContent
            anchors.fill: parent
            visible: !win.compactMode

            Item {
                id: titleBar
                x: 0
                y: 0
                width: parent.width
                height: 30

                Row {
                    anchors {
                        right: parent.right
                        rightMargin: 8
                        verticalCenter: parent.verticalCenter
                    }
                    spacing: 10

                    FlatIconButton {
                        width: 28
                        height: 28
                        glyph: "minus"
                        onClicked: win.compactMode = true
                    }

                    FlatIconButton {
                        width: 28
                        height: 28
                        glyph: "close"
                        hoverDanger: true
                        onClicked: win.close()
                    }
                }
            }

            Item {
                id: taskArea
                x: 18
                y: 38
                width: parent.width - 36
                height: 317
                clip: true
                visible: !win.showHistory

                SectionHeader {
                    id: pendingHeader
                    width: parent.width
                    height: 25
                    visible: win.selectedFilter !== "completed"
                    title: "PENDIENTES"
                    count: win.pendingCountValue
                }

                ListView {
                    id: taskListView
                    anchors {
                        left: parent.left
                        right: parent.right
                        top: parent.top
                        topMargin: pendingHeader.visible ? 33 : 0
                        bottom: parent.bottom
                    }
                    clip: true
                    spacing: 0
                    model: win.app.taskListModel
                    boundsBehavior: Flickable.StopAtBounds

                    delegate: Item {
                        id: rowHost
                        required property int taskId
                        required property string title
                        required property bool completed
                        required property string status
                        required property string phaseName
                        required property int sortOrder

                        width: ListView.view.width
                        readonly property bool isSection: status === "section-completed"
                        readonly property bool isDone: status === "completed" || completed
                        readonly property bool filteredIn: isSection
                            ? (win.selectedFilter !== "pending" && win.completedCountValue > 0)
                            : (win.selectedFilter === "all" || (win.selectedFilter === "pending" && !isDone) || (win.selectedFilter === "completed" && isDone))
                        readonly property bool collapsedOut: isDone && win.completedCollapsed && win.selectedFilter !== "pending"
                        height: filteredIn && !collapsedOut ? (isSection ? 37 : 58) : 0
                        visible: height > 0
                        clip: true

                        SectionHeader {
                            visible: rowHost.isSection
                            anchors {
                                left: parent.left
                                right: parent.right
                                top: parent.top
                            }
                            height: 32
                            title: "COMPLETADAS"
                            count: win.completedCountValue
                            showChevron: true
                            collapsed: win.completedCollapsed
                            onToggle: win.completedCollapsed = !win.completedCollapsed
                        }

                        TaskItem {
                            visible: !rowHost.isSection
                            anchors {
                                left: parent.left
                                right: parent.right
                                top: parent.top
                            }
                            height: 51
                            taskId: rowHost.taskId
                            title: rowHost.title
                            completed: rowHost.completed
                            status: rowHost.status
                            phaseName: rowHost.phaseName
                            sortOrder: rowHost.sortOrder
                            onSelected: function(tid) { win.app.selectTask(tid) }
                            onToggleComplete: function(tid) {
                                if (rowHost.completed)
                                    win.app.reopenTask(tid)
                                else
                                    win.app.completeTask(tid)
                            }
                        }
                    }
                }
            }

            HistoryPanel {
                visible: win.showHistory
                x: 18
                y: 38
                width: parent.width - 36
                height: 317
                onClose: win.showHistory = false
            }

            Rectangle {
                id: lowerDivider
                x: 12
                y: 356
                width: parent.width - 24
                height: 1
                color: Theme.borderSoft
            }

            Rectangle {
                id: inputBox
                x: 18
                y: 366
                width: parent.width - 76
                height: 40
                radius: 9
                color: Theme.panel
                border { color: Theme.borderSoft; width: 1 }

                TextField {
                    id: taskInput
                    anchors.fill: parent
                    anchors.leftMargin: 12
                    anchors.rightMargin: 12
                    color: Theme.text
                    placeholderText: "Agregar una nueva tarea..."
                    placeholderTextColor: Theme.mutedText
                    verticalAlignment: TextInput.AlignVCenter
                    font { pixelSize: 12; family: Theme.fontFamily }
                    padding: 0
                    background: null
                    onAccepted: {
                        if (text.trim().length > 0) {
                            win.app.createTask(text.trim())
                            text = ""
                            win.selectedFilter = "all"
                        }
                    }
                }
            }

            Rectangle {
                id: addButton
                x: parent.width - 50
                y: 366
                width: 32
                height: 40
                radius: 7
                color: addMouse.containsMouse ? Theme.accentPressed : Theme.accent

                IconGlyph {
                    anchors.centerIn: parent
                    width: 16
                    height: 16
                    glyph: "plus"
                    strokeColor: "#08211d"
                    lineWidth: 2
                }

                MouseArea {
                    id: addMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        if (taskInput.text.trim().length > 0) {
                            win.app.createTask(taskInput.text.trim())
                            taskInput.text = ""
                            win.selectedFilter = "all"
                        }
                    }
                }
            }

            OutlineActionButton {
                x: 18
                y: 410
                width: parent.width - 36
                label: "Archivar completadas (%1)".arg(win.completedCountValue)
                icon: "archive"
                accent: true
                enabled: win.completedCountValue > 0
                opacity: enabled ? 1 : 0.45
                onClicked: win.app.archiveCompleted()
            }

            Rectangle {
                id: footer
                x: 0
                y: parent.height - 25
                width: parent.width
                height: 25
                color: "#111b2a"
                opacity: 0.96

                Row {
                    x: 18
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 7

                    Rectangle {
                        anchors.verticalCenter: parent.verticalCenter
                        width: 7
                        height: 7
                        radius: 4
                        color: win.app.online ? Theme.success : Theme.pending
                    }

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: win.app.online ? "Conectado" : "Offline"
                        color: Theme.dimText
                        font { pixelSize: 10; family: Theme.fontFamily }
                    }
                }

                Text {
                    anchors {
                        right: parent.right
                        rightMargin: 15
                        verticalCenter: parent.verticalCenter
                    }
                    text: "Focus HUD v0.2.0"
                    color: Theme.dimText
                    font { pixelSize: 10; family: Theme.fontFamily }
                }
            }
        }

        Item {
            id: compactContent
            anchors.fill: parent
            visible: win.compactMode

            Text {
                x: 18
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width - 150
                text: win.app.hasActiveTask ? win.app.currentTaskTitle : "Focus HUD"
                color: Theme.text
                elide: Text.ElideRight
                font { pixelSize: 13; weight: Font.DemiBold; family: Theme.fontFamily }
            }

            Row {
                anchors {
                    right: compactControls.left
                    rightMargin: 14
                    verticalCenter: parent.verticalCenter
                }
                spacing: 7

                Rectangle {
                    anchors.verticalCenter: parent.verticalCenter
                    width: 7
                    height: 7
                    radius: 4
                    color: win.app.online ? Theme.success : Theme.pending
                }

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: win.totalCountValue
                    color: Theme.dimText
                    font { pixelSize: 11; weight: Font.DemiBold; family: Theme.fontFamily }
                }
            }

            Row {
                id: compactControls
                anchors {
                    right: parent.right
                    rightMargin: 9
                    verticalCenter: parent.verticalCenter
                }
                spacing: 8

                FlatIconButton {
                    width: 28
                    height: 28
                    glyph: "chevron-up"
                    onClicked: win.compactMode = false
                }

                FlatIconButton {
                    width: 28
                    height: 28
                    glyph: "close"
                    hoverDanger: true
                    onClicked: win.close()
                }
            }

            MouseArea {
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
                    right: compactControls.left
                }
                acceptedButtons: Qt.LeftButton
                cursorShape: Qt.PointingHandCursor
                onClicked: win.compactMode = false
                onPressed: win.startSystemMove()
            }
        }

        MouseArea {
            id: dragArea
            enabled: !win.compactMode
            height: 30
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                leftMargin: 8
                rightMargin: 76
            }
            acceptedButtons: Qt.LeftButton
            cursorShape: Qt.SizeAllCursor
            onPressed: win.startSystemMove()
        }
    }

    component FilterTab: Rectangle {
        id: tab
        required property string label
        required property int count
        property bool active: false
        signal clicked()

        radius: 8
        color: active ? Theme.accentDim : "transparent"
        border { color: active ? Theme.accent : "transparent"; width: 1 }

        Row {
            anchors.centerIn: parent
            spacing: 8

            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: tab.label
                color: tab.active ? Theme.accent : Theme.dimText
                font { pixelSize: 11; weight: tab.active ? Font.DemiBold : Font.Normal; family: Theme.fontFamily }
            }

            Rectangle {
                anchors.verticalCenter: parent.verticalCenter
                width: tab.active ? countLabel.implicitWidth : Math.max(15, countLabel.implicitWidth + 7)
                height: tab.active ? 15 : 16
                radius: 8
                color: tab.active ? "transparent" : "#1a2635"

                Text {
                    id: countLabel
                    anchors.centerIn: parent
                    text: tab.count
                    color: tab.active ? Theme.accent : Theme.dimText
                    font { pixelSize: 11; weight: Font.DemiBold; family: Theme.fontFamily }
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: tab.clicked()
        }
    }

    component SectionHeader: Item {
        id: section
        required property string title
        required property int count
        property bool showChevron: false
        property bool collapsed: false
        signal toggle()

        Text {
            anchors {
                left: parent.left
                leftMargin: 2
                verticalCenter: parent.verticalCenter
            }
            text: section.title + " · " + section.count
            color: Theme.dimText
            font { pixelSize: 11; weight: Font.Bold; letterSpacing: 1; family: Theme.fontFamily }
        }

        IconGlyph {
            visible: section.showChevron
            anchors {
                right: parent.right
                rightMargin: 4
                verticalCenter: parent.verticalCenter
            }
            width: 14
            height: 14
            glyph: section.collapsed ? "chevron-down" : "chevron-up"
            strokeColor: Theme.dimText
            lineWidth: 1.8
        }

        MouseArea {
            anchors.fill: parent
            enabled: section.showChevron
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: section.toggle()
        }
    }

    component SquareButton: Rectangle {
        id: control
        property string glyph: "search"
        signal clicked()

        width: 33
        height: 33
        radius: 8
        color: controlMouse.containsMouse ? Theme.cardHover : Theme.panel
        border { color: Theme.borderSoft; width: 1 }

        IconGlyph {
            anchors.centerIn: parent
            width: 18
            height: 18
            glyph: control.glyph
            strokeColor: Theme.dimText
            lineWidth: 1.8
        }

        MouseArea {
            id: controlMouse
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: control.clicked()
        }
    }

    component FlatIconButton: Rectangle {
        id: control
        property string glyph: "minus"
        property bool hoverDanger: false
        signal clicked()

        radius: 8
        color: controlMouse.containsMouse ? (hoverDanger ? "#39212a" : Theme.cardHover) : "transparent"

        IconGlyph {
            anchors.centerIn: parent
            width: 18
            height: 18
            glyph: control.glyph
            strokeColor: controlMouse.containsMouse && control.hoverDanger ? "#ff9aaa" : Theme.text
            lineWidth: 1.8
        }

        MouseArea {
            id: controlMouse
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: control.clicked()
        }
    }

    component SquareActionButton: Rectangle {
        id: button
        property string icon: "refresh"
        signal clicked()

        height: 39
        radius: 7
        color: mouse.containsMouse ? Theme.cardHover : Theme.panel
        border { color: Theme.borderSoft; width: 1 }

        IconGlyph {
            anchors.centerIn: parent
            width: 19
            height: 19
            glyph: button.icon
            strokeColor: Theme.text
            lineWidth: 1.8
        }

        MouseArea {
            id: mouse
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: button.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
            onClicked: if (button.enabled) button.clicked()
        }
    }

    component OutlineActionButton: Rectangle {
        id: button
        required property string label
        property string icon: "archive"
        property bool accent: false
        signal clicked()

        height: 39
        radius: 7
        color: accent ? (mouse.containsMouse ? "#173f39" : "#123631") : (mouse.containsMouse ? Theme.cardHover : Theme.panelDeep)
        border { color: accent ? Theme.accentPressed : Theme.borderSoft; width: 1 }

        Row {
            anchors.centerIn: parent
            spacing: 9

            IconGlyph {
                anchors.verticalCenter: parent.verticalCenter
                width: 17
                height: 17
                glyph: button.icon
                strokeColor: button.accent ? Theme.accent : Theme.text
                lineWidth: 1.7
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: button.label
                color: button.accent ? Theme.accent : Theme.text
                font { pixelSize: 12; weight: Font.Bold; family: Theme.fontFamily }
            }
        }

        MouseArea {
            id: mouse
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: button.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
            onClicked: if (button.enabled) button.clicked()
        }
    }

    component AppLogo: Rectangle {
        id: logo
        radius: 7
        color: Theme.panel
        border { color: Theme.border; width: 1 }

        IconGlyph {
            anchors.centerIn: parent
            width: 18
            height: 18
            glyph: "brand-checks"
            strokeColor: Theme.accent
            lineWidth: 3
        }
    }

    component IconGlyph: Canvas {
        id: icon
        property string glyph: "minus"
        property color strokeColor: Theme.text
        property color fillColor: "transparent"
        property real lineWidth: 1.8

        onGlyphChanged: requestPaint()
        onStrokeColorChanged: requestPaint()
        onFillColorChanged: requestPaint()
        onLineWidthChanged: requestPaint()
        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)
            ctx.lineCap = "round"
            ctx.lineJoin = "round"
            ctx.lineWidth = lineWidth
            ctx.strokeStyle = strokeColor
            ctx.fillStyle = fillColor

            if (glyph === "minus") {
                ctx.beginPath()
                ctx.moveTo(width * 0.24, height * 0.5)
                ctx.lineTo(width * 0.76, height * 0.5)
                ctx.stroke()
            } else if (glyph === "close") {
                ctx.beginPath()
                ctx.moveTo(width * 0.22, height * 0.22)
                ctx.lineTo(width * 0.78, height * 0.78)
                ctx.moveTo(width * 0.78, height * 0.22)
                ctx.lineTo(width * 0.22, height * 0.78)
                ctx.stroke()
            } else if (glyph === "pin") {
                ctx.save()
                ctx.translate(width * 0.5, height * 0.5)
                ctx.rotate(Math.PI / 4)
                ctx.translate(-width * 0.5, -height * 0.5)
                ctx.beginPath()
                ctx.moveTo(width * 0.50, height * 0.12)
                ctx.lineTo(width * 0.50, height * 0.42)
                ctx.moveTo(width * 0.32, height * 0.42)
                ctx.lineTo(width * 0.68, height * 0.42)
                ctx.moveTo(width * 0.38, height * 0.42)
                ctx.lineTo(width * 0.32, height * 0.68)
                ctx.lineTo(width * 0.68, height * 0.68)
                ctx.lineTo(width * 0.62, height * 0.42)
                ctx.moveTo(width * 0.50, height * 0.68)
                ctx.lineTo(width * 0.50, height * 0.88)
                ctx.stroke()
                ctx.restore()
            } else if (glyph === "search") {
                ctx.beginPath()
                ctx.arc(width * 0.44, height * 0.42, width * 0.24, 0, Math.PI * 2)
                ctx.stroke()
                ctx.beginPath()
                ctx.moveTo(width * 0.62, height * 0.61)
                ctx.lineTo(width * 0.82, height * 0.82)
                ctx.stroke()
            } else if (glyph === "sliders") {
                for (var i = 0; i < 3; ++i) {
                    var yy = height * (0.27 + i * 0.23)
                    ctx.beginPath()
                    ctx.moveTo(width * 0.20, yy)
                    ctx.lineTo(width * 0.80, yy)
                    ctx.stroke()
                }
                ctx.fillStyle = strokeColor
                ctx.fillRect(width * 0.38, height * 0.23, 3, 3)
                ctx.fillRect(width * 0.56, height * 0.46, 3, 3)
                ctx.fillRect(width * 0.30, height * 0.69, 3, 3)
            } else if (glyph === "plus") {
                ctx.beginPath()
                ctx.moveTo(width * 0.50, height * 0.20)
                ctx.lineTo(width * 0.50, height * 0.80)
                ctx.moveTo(width * 0.20, height * 0.50)
                ctx.lineTo(width * 0.80, height * 0.50)
                ctx.stroke()
            } else if (glyph === "chevron-down") {
                ctx.beginPath()
                ctx.moveTo(width * 0.22, height * 0.38)
                ctx.lineTo(width * 0.50, height * 0.64)
                ctx.lineTo(width * 0.78, height * 0.38)
                ctx.stroke()
            } else if (glyph === "chevron-up") {
                ctx.beginPath()
                ctx.moveTo(width * 0.22, height * 0.62)
                ctx.lineTo(width * 0.50, height * 0.36)
                ctx.lineTo(width * 0.78, height * 0.62)
                ctx.stroke()
            } else if (glyph === "archive") {
                ctx.strokeRect(width * 0.18, height * 0.30, width * 0.64, height * 0.50)
                ctx.beginPath()
                ctx.moveTo(width * 0.14, height * 0.25)
                ctx.lineTo(width * 0.86, height * 0.25)
                ctx.lineTo(width * 0.78, height * 0.12)
                ctx.lineTo(width * 0.22, height * 0.12)
                ctx.closePath()
                ctx.stroke()
                ctx.beginPath()
                ctx.moveTo(width * 0.40, height * 0.48)
                ctx.lineTo(width * 0.60, height * 0.48)
                ctx.stroke()
            } else if (glyph === "history" || glyph === "refresh") {
                ctx.beginPath()
                ctx.arc(width * 0.52, height * 0.53, width * 0.30, Math.PI * 0.08, Math.PI * 1.68)
                ctx.stroke()
                ctx.beginPath()
                ctx.moveTo(width * 0.25, height * 0.18)
                ctx.lineTo(width * 0.25, height * 0.38)
                ctx.lineTo(width * 0.43, height * 0.36)
                ctx.stroke()
                if (glyph === "history") {
                    ctx.beginPath()
                    ctx.moveTo(width * 0.52, height * 0.53)
                    ctx.lineTo(width * 0.52, height * 0.34)
                    ctx.moveTo(width * 0.52, height * 0.53)
                    ctx.lineTo(width * 0.65, height * 0.61)
                    ctx.stroke()
                }
            } else if (glyph === "brand-checks") {
                ctx.beginPath()
                ctx.moveTo(width * 0.12, height * 0.48)
                ctx.lineTo(width * 0.30, height * 0.64)
                ctx.lineTo(width * 0.58, height * 0.30)
                ctx.moveTo(width * 0.42, height * 0.63)
                ctx.lineTo(width * 0.56, height * 0.76)
                ctx.lineTo(width * 0.88, height * 0.25)
                ctx.stroke()
            }
        }
    }
}
