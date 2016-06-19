pragma Singleton
import QtQuick 2.0
import Material 0.3

QtObject {
    id: constValue
//    readonly property int screenWidth: Units.dp * 1280
//    readonly property int screenHeight: Units.dp * 800
    readonly property real leftEdgeMargins: Units.dp * 24
    readonly property real itemHeight: Units.dp * 48
    readonly property real subHeaderHeight: Units.dp * 40
    readonly property real tinySpace: Units.dp * 8
    readonly property real middleSpace: Units.dp * 16
    readonly property real largeSpace: Units.dp * 24
//    readonly property real cardSize: Units.dp * 156
    //rightSidebarWidth width copyed from material lib
//    readonly property real rightSidebarWidth: 250 * Units.dp

//    readonly property string localMusicCtrlUid: "bibibibibi-this-is-localCtrl"

}
