pragma Singleton
import QtQuick

QtObject {
    readonly property color bg:       "#0f0f1a"
    readonly property color card:     "#1a1a2e"
    readonly property color border:   "#2a2a4a"
    readonly property color accent:   "#00d4aa"
    readonly property color text:     "#e0e0e0"
    readonly property color dimText:  "#8888aa"
    readonly property color success:  "#00d4aa"
    readonly property color pending:  "#ff6b6b"
    readonly property color badge:    "#00d4aa"

    readonly property int radius:     14
    readonly property int padding:    20
    readonly property int spacing:    12
    readonly property font family:    "Segoe UI, 'Helvetica Neue', Arial, sans-serif"
}
