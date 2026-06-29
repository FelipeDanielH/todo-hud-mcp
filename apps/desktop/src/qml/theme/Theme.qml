pragma Singleton
import QtQuick

QtObject {
    readonly property color bg:       "#101118"
    readonly property color card:     "#191b27"
    readonly property color border:   "#303548"
    readonly property color accent:   "#22d3a6"
    readonly property color accentPressed: "#16b894"
    readonly property color text:     "#f2f4f8"
    readonly property color dimText:  "#9aa3b8"
    readonly property color success:  "#22d3a6"
    readonly property color pending:  "#ff6b6b"
    readonly property color badge:    "#22d3a6"

    readonly property int radius:     12
    readonly property int padding:    18
    readonly property int spacing:    10
    readonly property string fontFamily: "Segoe UI"
}
