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

    Component.onCompleted: {
        SearchStore.clear();
    }

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
    model: SearchStore.searchModel
    delegate: ListItem.Subtitled {
        id: searchItem
        property var object: SearchStore.searchModel[index]
        property bool isDir: object[FileObjectKey.keyIsdir] == 1
        property string path: object[FileObjectKey.keyPath]
        property string fileName: AppUtility.fileObjectPathToFileName(path)
        property string mtime: object[FileObjectKey.keyServerMTime]
        property string fsID: object[FileObjectKey.keyFsId]
        property int category: object[FileObjectKey.keyCategory]
        property int size: object[FileObjectKey.keySize]

        showDivider: true
        iconName: isDir ? "file/folder" : Utility.categoryToIcon(category)
        text: fileName
        subText: {
            var v = isDir ? qsTr("Dir") : AppUtility.sizeToStr(size)
            v += " - ";
            v += AppUtility.formatDate(mtime);
            v += " - ";
            v += path;
            return v;
        }

        secondaryItem: Row {
            height: childrenRect.height
            anchors.verticalCenter: parent.verticalCenter
            spacing: Const.middleSpace
            IconButton {
                visible: !searchItem.isDir
                enabled: visible
                action: Action {
                    iconName: "file/file_download"
                }
                onClicked: {
                    if (!dirItem.isDir) {
                        AppActions.askToSelectDownloadPath(path, fileName);
                    }
                }
            }
            IconButton {
                action: Action {
                    iconName: "social/share"
                }
                onClicked: {
                    shareMenuClicked(searchItem, searchItem.fsID);
                }
            }
            IconButton {
                action: Action {
                    iconName: "navigation/more_vert"
                }
                onClicked: {
                    moreVertMenuClicked(searchItem, searchItem.path, searchItem.isDir);
                }
            }
        }
//        onClicked: {
//            if (dirItem.isDir) {
//                AppActions.showDir(object[FileObjectKey.keyPath]);
//            }
//        }
    }

    Scrollbar {
        flickableItem: parent
    }
}
