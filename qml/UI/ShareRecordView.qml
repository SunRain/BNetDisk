import QtQuick 2.4
import Material 0.3
import Material.ListItems 0.1 as ListItem

import com.sunrain.bnetdisk.qmlplugin 1.0

import ".."
import "../QuickFlux/Stores"
import "../QuickFlux/Actions"
import "../QuickFlux/Scripts"
import "../Component"

import "../Script/Utility.js" as Utility

ListView {
    id: content
    width: parent.width
    height: parent.height

//    signal shareMenuClicked(var parentItem, var id)
//    signal moreVertMenuClicked(var parentItem, var path, var isDir)

    clip: true
    interactive: contentHeight > height
    onContentXChanged: {
        if(contentX != 0 && contentWidth <= width)
            contentX = 0
    }
    onContentYChanged: {
        if(contentY != 0 && contentHeight <= height)
            contentY = 0
    }
    spacing: Const.tinySpace

    PullToRefresh {
        refreshing: ShareStore.refreshing
        onRefresh: {
            AppActions.showShareRecord("1");
        }
    }

    model: ShareStore.shareRecordList
    delegate: ListItem.Subtitled {
        id: item
        property var object: ShareStore.shareRecordList[index]
        property string shareId: object["shareId"]
        property string passwd: object["passwd"]
        property string path: object["typicalPath"]
        property string fileName: AppUtility.fileObjectPathToFileName(path)
        property string shortlink: object["shortlink"]
        property int typicalCategory: object["typicalCategory"]
        property bool isDir: typicalCategory == -1 ? true : false
        property bool isPublic: object["public"] == 1 ? true : false
        interactive: true
        showDivider: true
//        iconName: isDir ? "file/folder" : Utility.categoryToIcon(typicalCategory)
        text: fileName
//        subText: shortlink
        action: Row {
            height: parent.height
            Icon {
                anchors.verticalCenter: parent.verticalCenter
                color: item.selected ? Theme.primaryColor : Theme.light.iconColor
                size: 24 * Units.dp
                name: item.isPublic ? "" : "action/lock"
            }
            Icon {
                anchors.verticalCenter: parent.verticalCenter
                color: item.selected ? Theme.primaryColor : Theme.light.iconColor
                size: 24 * Units.dp
                name: item.isDir ? "file/folder" : Utility.categoryToIcon(typicalCategory)
            }
        }
        content: Item {
            anchors.fill: parent
            Row {
                height: parent.height
                spacing: Const.tinySpace
                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr("Link")+": "
                }
                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    text: item.shortlink
                    color: Theme.primaryColor
                }
                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr("Password") + ": "
                    enabled: !item.isPublic
                    visible: !item.isPublic
                }
                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    text: item.passwd
                    enabled: !item.isPublic
                    visible: !item.isPublic
                }
            }
            Row {
                height: parent.height
                spacing: Const.tinySpace
                anchors.right: parent.right
                anchors.rightMargin: Const.largeSpace
                Button {
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr("Copy to clipboard")
                    textColor: Theme.accentColor
                    onClicked: {
                        var text = item.shortlink;
                        if (!item.isPublic) {
                            text += " "
                            text += qsTr("password")
                            text += ": "
                            text += item.passwd
                        }
                        AppUtility.copyToClipboard(text);
                        AppActions.snackbarInfo(qsTr("Copyed!!"))
                    }
                }
                IconButton {
                    action: Action {
                        iconName: "navigation/cancel"
                    }
                    onClicked: {
                        AppActions.askToCancelShare(item.shareId);
                    }
                }
            }
        }
    }
    Scrollbar {
        flickableItem: parent
    }
}
