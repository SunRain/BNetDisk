import QtQuick 2.4
import Material 0.3
import Material.ListItems 0.1 as ListItem

import com.sunrain.bnetdisk.qmlplugin 1.0

import ".."
import "../QuickFlux/Stores"
import "../QuickFlux/Actions"
import "../QuickFlux/Scripts"

import "../Script/Utility.js" as Utility

ListView {
    id: content
    width: parent.width
    height: parent.height

    signal shareMenuClicked(var parentItem, var id)
    signal moreVertMenuClicked(var parentItem, var path, var isDir)

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
    spacing: dp(8)
    model: DirListStore.dirlistModel
    delegate: ListItem.Subtitled {
        id: dirItem
        property var object: DirListStore.dirlistModel[index] //.get(index)
        property bool isDir: object[FileObjectKey.keyIsdir] == 1
        property string path: object[FileObjectKey.keyPath]
        property string fileName: AppUtility.fileObjectPathToFileName(path)
        property string mtime: object[FileObjectKey.keyServerMTime]
        property string fsID: object[FileObjectKey.keyFsId]
        property int category: object[FileObjectKey.keyCategory]
        property int size: object[FileObjectKey.keySize]
        property var leftTime: object[FileObjectKey.keyLeftTime]
        property bool trashView: leftTime != undefined && leftTime != ""

        showDivider: true
        iconName: isDir ? "file/folder" : Utility.categoryToIcon(category)
        text: fileName
        subText: {
            var v = isDir ? qsTr("Dir") : AppUtility.sizeToStr(size)
            v += " - ";
            if (trashView) {
                v += qsTr("Delete Date") + ": ";
                v += AppUtility.formatDate(mtime);
                v += " - ";
                v += qsTr("Left Time") + ": ";
                v += leftTime + qsTr("Days");
            } else {
                v += AppUtility.formatDate(mtime);
            }
            return v;
        }
        secondaryItem: Row {
            height: childrenRect.height
            anchors.verticalCenter: parent.verticalCenter
            spacing: Const.middleSpace
            IconButton {
                visible: !dirItem.isDir && !dirItem.trashView
                enabled: visible
                action: Action {
                    iconName: "file/file_download"
                }
                onClicked: {
                    console.log("=== download file");
                    if (!dirItem.isDir) {
                        AppActions.askToSelectDownloadPath(path, fileName);
                    }
                }
            }
            IconButton {
                visible: !dirItem.trashView
                enabled: visible
                action: Action {
                    iconName: "social/share"
                }
                onClicked: {
                    console.log('=========== fsid '+dirItem.fsID);
                    shareMenuClicked(dirItem, dirItem.fsID);
                }
            }
            IconButton {
                visible: !dirItem.trashView
                enabled: visible
                action: Action {
                    iconName: "navigation/more_vert"
                }
                onClicked: {
                    moreVertMenuClicked(dirItem, dirItem.path, dirItem.isDir);
                }
            }
            IconButton {
                visible: dirItem.trashView
                enabled: dirItem.trashView
                action: Action {
                    iconName: "content/undo"
                }
                onClicked: {
                    AppActions.askToRecycleRestore(dirItem.fsID);
                }
            }
//            IconButton {
//                visible: dirItem.trashView
//                enabled: visible
//                action: Action {
//                    iconName: "action/delete"
//                }
//            }
        }
        onClicked: {
            if (dirItem.isDir) {
                AppActions.showDir(object[FileObjectKey.keyPath]);
            }
        }
    }

    Scrollbar {
        flickableItem: parent
    }
}
