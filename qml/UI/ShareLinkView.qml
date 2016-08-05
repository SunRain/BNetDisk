import QtQuick 2.4
import Material 0.3

import com.sunrain.bnetdisk.qmlplugin 1.0

import ".."
import "../QuickFlux/Stores"

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
        TextField {
            width: parent.width
            text: ShareStore.sharelink
            placeholderText: qsTr("share link")
            floatingLabel: true
            visible: ShareStore.sharelink != ""
        }
        TextField {
            width: parent.width
            text: ShareStore.password
            placeholderText: qsTr("password")
            floatingLabel: true
            visible: ShareStore.password
        }
        Item {
            height: Const.middleSpace
        }
    }
}
