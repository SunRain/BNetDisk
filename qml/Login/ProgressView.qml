import QtQuick 2.0
import Material 0.3

Item {
    id: progressView
    anchors.fill: parent
    ProgressCircle {
        id: cyclicColorProgress
        anchors.centerIn: parent
        width: height
        height: parent.height * 0.4
        dashThickness: dp(8)
        SequentialAnimation {
            running: true
            loops: Animation.Infinite

            ColorAnimation {
                from: "red"
                to: "blue"
                target: cyclicColorProgress
                properties: "color"
                easing.type: Easing.InOutQuad
                duration: 2400
            }

            ColorAnimation {
                from: "blue"
                to: "green"
                target: cyclicColorProgress
                properties: "color"
                easing.type: Easing.InOutQuad
                duration: 1560
            }

            ColorAnimation {
                from: "green"
                to: "#FFCC00"
                target: cyclicColorProgress
                properties: "color"
                easing.type: Easing.InOutQuad
                duration:  840
            }

            ColorAnimation {
                from: "#FFCC00"
                to: "red"
                target: cyclicColorProgress
                properties: "color"
                easing.type: Easing.InOutQuad
                duration:  1200
            }
        }
    }
}

