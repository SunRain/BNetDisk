import QtQuick 2.4
import Material 0.3

import com.sunrain.bnetdisk.qmlplugin 1.0

import ".."
import "../QuickFlux/Stores"
import "../QuickFlux/Actions"

Item {
    id: shareLinkItem
    width: parent.width
    height: column.height
    Column {
        id: column
        width: parent.width - Const.middleSpace * 2
        anchors.centerIn: parent
        spacing: Const.middleSpace * 2

        Item {
            height: Const.middleSpace
        }
        Label {
            width: parent.width
            anchors.horizontalCenter: parent.horizontalCenter
            style: "title"
            text: qsTr("share link")
        }
        Label {
            width: parent.width
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Share error");
            visible: ShareStore.showErrorLabel
        }
        Item {
            width: parent.width
            height: Const.itemHeight
            visible: ShareStore.sharelink != ""
            enabled: visible
            Row {
                anchors.left: parent.left
                height: parent.height
                spacing: Const.tinySpace
                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr("Link") + ": "
                }
                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    text: ShareStore.sharelink
                    color: Theme.primaryColor
                }
                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr("Password") + ": "
                    enabled: ShareStore.password
                    visible: enabled
                }
                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    text: ShareStore.password
                    enabled: ShareStore.password
                    visible: enabled
                }
            }
            Button {
                anchors {
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }
                text: qsTr("Copy to clipboard")
                textColor: Theme.accentColor
                onClicked: {
                    var text = ShareStore.sharelink
                    if (ShareStore.password != "") {
                        text += " "
                        text += qsTr("password")
                        text += ": "
                        text += ShareStore.password
                    }
                    AppUtility.copyToClipboard(text);
                    AppActions.snackbarInfo(qsTr("Copyed!!"))
                }
            }
        }
        Item {
            height: Const.middleSpace
        }
    }
}
