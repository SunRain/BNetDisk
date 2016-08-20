import QtQuick 2.4
import QtQuick.Controls 1.3 as Controls
import QtQuick.Controls.Styles 1.3
import QtQuick.Layouts 1.1
import Material 0.3

Controls.TextField {
    property color color: Theme.accentColor
    property color errorColor: Palette.colors["red"]["500"]
    property string helperText
    property bool floatingLabel: false
    property bool hasError: characterLimit && length > characterLimit
    property int characterLimit
    property bool showBorder: true

    property Item action: null
    property Item secondaryItem: null

    style: TextFieldStyle {
        id: style

        property real leftPadding: control.hasOwnProperty("action") && control.action
                                   ? control.action.width + 8 * Units.dp
                                   : 0
        property real rightPadding: control.hasOwnProperty("secondaryItem") && control.secondaryItem
                                    ? control.secondaryItem.width + 8 * Units.dp
                                    : 0

        padding {
            left: style.leftPadding
            right: style.rightPadding
            top: 0
            bottom: 0
        }

        font {
            family: echoMode == TextInput.Password ? "Default" : "Roboto"
            pixelSize: 16 * Units.dp
        }

        renderType: Text.QtRendering
        placeholderTextColor: "transparent"
        selectedTextColor: "white"
        selectionColor: control.hasOwnProperty("color") ? control.color : Theme.accentColor
        textColor: Theme.light.textColor

        background : Item {
            id: background

            property color color: control.hasOwnProperty("color") ? control.color : Theme.accentColor
            property color errorColor: control.hasOwnProperty("errorColor")
                    ? control.errorColor : Palette.colors["red"]["500"]
            property string helperText: control.hasOwnProperty("helperText") ? control.helperText : ""
            property bool floatingLabel: control.hasOwnProperty("floatingLabel") ? control.floatingLabel : ""
            property bool hasError: control.hasOwnProperty("hasError")
                    ? control.hasError : characterLimit && control.length > characterLimit
            property int characterLimit: control.hasOwnProperty("characterLimit") ? control.characterLimit : 0
            property bool showBorder: control.hasOwnProperty("showBorder") ? control.showBorder : true

            Rectangle {
                id: underline
                color: background.hasError ? background.errorColor
                                        : control.activeFocus ? background.color
                                                              : Theme.light.hintColor

                height: control.activeFocus ? 2 * Units.dp : 1 * Units.dp
                visible: background.showBorder

                anchors {
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                }

                Behavior on height {
                    NumberAnimation { duration: 200 }
                }

                Behavior on color {
                    ColorAnimation { duration: 200 }
                }
            }

            Item {
                id: actionContent
                anchors {
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                }
                children: [control.action]
                width: control.hasOwnProperty("action") && control.action ? control.action.width : 0
                height: control.hasOwnProperty("action") && control.action ? control.action.height : 0
            }

            Item {
                id: secondaryItemContent
                anchors {
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }
                children: [control.secondaryItem]
                width: control.hasOwnProperty("secondaryItem") && control.secondaryItem
                       ? control.secondaryItem.width
                       : 0
                height: control.hasOwnProperty("secondaryItem") && control.secondaryItem
                        ? control.secondaryItem.height
                        : 0
            }


            Label {
                id: fieldPlaceholder

                anchors.verticalCenter: parent.verticalCenter
                text: control.placeholderText
                font.pixelSize: 16 * Units.dp
                anchors.margins: -12 * Units.dp
                anchors.left: actionContent.right
                anchors.right: secondaryItemContent.left

                color: background.hasError ? background.errorColor
                                      : control.activeFocus && control.text !== ""
                                            ? background.color : Theme.light.hintColor

                states: [
                    State {
                        name: "floating"
                        when: control.displayText.length > 0 && background.floatingLabel
                        AnchorChanges {
                            target: fieldPlaceholder
                            anchors.verticalCenter: undefined
                            anchors.top: parent.top
                        }
                        PropertyChanges {
                            target: fieldPlaceholder
                            font.pixelSize: 12 * Units.dp
                        }
                    },
                    State {
                        name: "hidden"
                        when: control.displayText.length > 0 && !background.floatingLabel
                        PropertyChanges {
                            target: fieldPlaceholder
                            visible: false
                        }
                    }
                ]

                transitions: [
                    Transition {
                        id: floatingTransition
                        enabled: false
                        AnchorAnimation {
                            duration: 200
                        }
                        NumberAnimation {
                            duration: 200
                            property: "font.pixelSize"
                        }
                    }
                ]

                Component.onCompleted: floatingTransition.enabled = true
            }

            RowLayout {
                anchors {
                    left: parent.left
                    right: parent.right
                    top: underline.top
                    topMargin: 4 * Units.dp
                }

                Label {
                    id: helperTextLabel
                    visible: background.helperText && background.showBorder
                    text: background.helperText
                    font.pixelSize: 12 * Units.dp
                    color: background.hasError ? background.errorColor
                                               : Qt.darker(Theme.light.hintColor)

                    Behavior on color {
                        ColorAnimation { duration: 200 }
                    }

                    property string helperText: control.hasOwnProperty("helperText")
                            ? control.helperText : ""
                }

                Label {
                    id: charLimitLabel
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                    visible: background.characterLimit && background.showBorder
                    text: control.length + " / " + background.characterLimit
                    font.pixelSize: 12 * Units.dp
                    color: background.hasError ? background.errorColor : Theme.light.hintColor
                    horizontalAlignment: Text.AlignLeft

                    Behavior on color {
                        ColorAnimation { duration: 200 }
                    }
                }
            }
        }
    }
}
