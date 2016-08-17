import QtQuick 2.4
import Material 0.3
import Material.ListItems 0.1 as ListItem

import com.sunrain.bnetdisk.qmlplugin 1.0

import ".."
import "../QuickFlux/Stores"
import "../QuickFlux/Actions"
import "../QuickFlux/Scripts"
import "../UI"

import "../Script/Utility.js" as Utility

Page {
    id: searchPage

    property string searchKey

    actions: [
        Action{
            iconName: "action/search"
            name: qsTr("Search")
            onTriggered: {
                AppActions.search(searchKey);
            }
        }
    ]

    actionBar.customContent: Item {
        anchors.fill: parent
        TextField {
            anchors.verticalCenter: parent.verticalCenter
            text: ""
            onTextChanged: {
                searchKey = text;
            }
            onAccepted: {
                console.log("====== onAccepted "+searchKey)
            }
        }
    }

    Component.onCompleted: {
        SearchStore.clear();
    }

    ListView {
        id: listView
        anchors.fill: parent
        clip: true
        spacing: Const.tinySpace
        model: SearchStore.searchModel
        Scrollbar {
            flickableItem: parent
        }
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
                        dropDownMenu.shareId = searchItem.fsID;
                        dropDownMenu.parent = searchItem;
                        dropDownMenu.showDelete = false;
                        dropDownMenu.showRename = false;
                        dropDownMenu.showDelete = false;
                        dropDownMenu.showShare = true;
                        dropDownMenu.open(searchItem, 0, 0);
                    }
                }
                IconButton {
                    action: Action {
                        iconName: "navigation/more_vert"
                    }
                    onClicked: {
                        dropDownMenu.path = searchItem.path;
                        dropDownMenu.parent = searchItem;
                        dropDownMenu.showDownload = false;
                        dropDownMenu.showShare = false;
                        dropDownMenu.showDelete = true;
                        dropDownMenu.showRename = true;
                        dropDownMenu.open(searchItem, 0, 0);
                    }
                }
            }
        }
    }

    Dropdown {
        id: dropDownMenu
        anchor: Item.TopRight
        width:  searchPage.width /4
        height: moreVertColumn.height

        property string path: ""
        property string fileName: ""

        property string shareId: ""

        property bool showShare: false
        property bool showDownload: false
        property bool showRename: true
        property bool showDelete: true

        Rectangle {
            anchors.fill: parent
            radius: 2 * Units.dp
        }
        Column {
            id: moreVertColumn
            width: parent.width
            ListItem.Standard {
                text: qsTr("download")
                enabled: dropDownMenu.showDownload
                visible: dropDownMenu.showDownload
                onClicked: {
                    AppActions.askToSelectDownloadPath(dropDownMenu.path, dropDownMenu.fileName);
                    dropDownMenu.close();
                }
            }
            ListItem.Standard {
                text: qsTr("rename")
                enabled: dropDownMenu.showRename
                visible: dropDownMenu.showRename
                onClicked: {
                    AppActions.askToRename(dropDownMenu.path);
                    dropDownMenu.close();
                }
            }
            ListItem.Standard {
                text: qsTr("delete")
                enabled: dropDownMenu.showDelete
                visible: dropDownMenu.showDelete
                onClicked: {
                    AppActions.askToDelete(dropDownMenu.path);
                    dropDownMenu.close();
                }
            }
            ListItem.Standard {
                enabled: dropDownMenu.showShare
                visible: dropDownMenu.showShare
                text: qsTr("share public")
                onClicked: {
                    AppActions.openShareOverlayView();
                    AppActions.pubShare(dropDownMenu.shareId);
                    overlayView.open(mainViewPage);
                    dropDownMenu.close();
                }
            }
            ListItem.Standard {
                enabled: dropDownMenu.showShare
                visible: dropDownMenu.showShare
                text: qsTr("share private")
                onClicked: {
                    AppActions.openShareOverlayView();
                    AppActions.privShare(dropDownMenu.shareId);
                    overlayView.open(mainViewPage);
                    dropDownMenu.close();
                }
            }
        }
    }
}
