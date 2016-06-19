import QtQuick 2.4
import Material 0.3
import Material.ListItems 0.1 as ListItem

import com.sunrain.bnetdisk.qmlplugin 1.0

import ".."
import "../QuickFlux/Stores"
import "../QuickFlux/Actions"

import "../Script/Utility.js" as Utility

Page {
    id: mainViewPage

    actions: [
        Action {
            iconName: "action/search"
            name: qsTr("Search")
            onTriggered: {
//                musicLibraryManager.scanLocalMusic();
//                AppActions.selectMusicScannerDirs();
            }
        },
        Action {
            iconName: "action/settings"
            name: qsTr("Settings")
        }
    ]

    backAction: Action {
        iconName: "navigation/menu"
        visible: true
        onTriggered: {
            sidebar.expanded = !sidebar.expanded
        }
    }

    // use an Item for extendedContent wrapper
//    actionBar.extendedContent: Item {
//        width: parent.width
//        height: dp(48)
//        Item {
//            x: (parent.width - width) +16 * Units.dp // 16 * Units.dp from ActionBar->label topMargin
//            width: actionBar.width
//            height: parent.height
//            Row {
//                id: navRow
//                spacing: Const.largeSpace
//                height: childrenRect.height
//                anchors {
//                    left: parent.left
//                    leftMargin: 16 * Units.dp //from ActionBar->leftItem
//                    verticalCenter: parent.verticalCenter
//                }
//                IconButton {
//                    color: Theme.lightDark(theme.primaryColor,
//                                           Theme.light.iconColor, Theme.dark.iconColor)
//                    action: Action {
//                        iconName: "navigation/arrow_back"
//                    }
//                }
//                IconButton {
//                    color: Theme.lightDark(theme.primaryColor,
//                                           Theme.light.iconColor, Theme.dark.iconColor)
//                    action: Action {
//                        iconName: "navigation/arrow_forward"
//                    }
//                }
//                IconButton {
//                    color: Theme.lightDark(theme.primaryColor,
//                                           Theme.light.iconColor, Theme.dark.iconColor)
//                    action: Action {
//                        iconName: "navigation/arrow_upward"
//                    }
//                }
//                IconButton {
//                    color: Theme.lightDark(theme.primaryColor,
//                                           Theme.light.iconColor, Theme.dark.iconColor)
//                    action: Action {
//                        iconName: "navigation/refresh"
//                    }
//                }
//            }
//            View {
//                radius: 2
//                anchors {
//                    left: navRow.right
//                    leftMargin: Const.largeSpace
//                    right: parent.right
//                    rightMargin: Const.largeSpace
//                }
//                height: parent.height
//                TextField {
//                    id: addrText
//                    width: parent.width
//                    text: "Here/bdisk/file/path"
//                    anchors {
////                        bottom: parent.bottom
//                        verticalCenter: parent.verticalCenter
//                    }

//                    onAccepted: {
//                        console.log("=== addrText onAccepted "+text);
//                    }
//                }
//            }

//        }
//    }

    actionBar.customContent: Item {
        anchors.fill: parent
        Row {
            id: navRow
            spacing: Const.largeSpace
            height: childrenRect.height
            anchors {
                left: parent.left
                leftMargin: 16 * Units.dp //from ActionBar->leftItem
                verticalCenter: parent.verticalCenter
            }
//            IconButton {
//                color: Theme.lightDark(theme.primaryColor,
//                                       Theme.light.iconColor, Theme.dark.iconColor)
//                action: Action {
//                    iconName: "navigation/arrow_back"
//                }
//            }
//            IconButton {
//                color: Theme.lightDark(theme.primaryColor,
//                                       Theme.light.iconColor, Theme.dark.iconColor)
//                action: Action {
//                    iconName: "navigation/arrow_forward"
//                }
//            }
            IconButton {
                color: Theme.lightDark(theme.primaryColor,
                                       Theme.light.iconColor, Theme.dark.iconColor)
                action: Action {
                    iconName: "navigation/arrow_upward"
                    onTriggered: {
                        console.log("=== >>> cdup");
                        AppActions.cdup();
                    }
                }
            }
            IconButton {
                color: Theme.lightDark(theme.primaryColor,
                                       Theme.light.iconColor, Theme.dark.iconColor)
                action: Action {
                    iconName: "navigation/refresh"
                    onTriggered: {
                        AppActions.refreshCurrentDir();
                    }
                }
            }
        }
        View {
            radius: 2
            anchors {
                left: navRow.right
                leftMargin: Const.largeSpace
                right: parent.right
                rightMargin: Const.largeSpace
            }
            height: parent.height
            TextField {
                id: addrText
                width: parent.width
                text: DirListStore.currentPath//"Here/bdisk/file/path"
                anchors.verticalCenter: parent.verticalCenter
                onAccepted: {
                    console.log("=== addrText onAccepted "+text);
                }
            }
        }
    }

    Sidebar {
        id: sidebar
        property var nameList: [qsTr("All"), qsTr("Image"), qsTr("Document"), qsTr("Video"),
            qsTr("BT"), qsTr("Audio"), qsTr("Other")]
        property var iconList: ["file/attachment", "image/image", "image/texture", "av/movie",
        "file/attachment", "image/music_note", "file/attachment"]
        Column {
            width: parent.width
            Repeater {
                model: sidebar.nameList.length
                delegate: ListItem.Standard {
                    text: sidebar.nameList[index]
                    iconName: sidebar.iconList[index]
                }
            }
            ListItem.Divider{}
            ListItem.Standard {
                text: qsTr("My Share")
                iconName: "social/share"
            }
            ListItem.Divider{}
            ListItem.Standard {
                text: "Trash"
                iconName: "action/delete"
            }
        }
    }

    Flickable {
        id: content
        width: parent.width
        anchors {
            top: parent.top
            left: sidebar.right
            leftMargin: Const.tinySpace
            right: parent.right
            rightMargin: Const.tinySpace
            bottom: parent.bottom
        }
//        contentWidth: parent.width
        contentHeight: column.height
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
        Column {
            id: column
            width: parent.width
            spacing: dp(8)

            Repeater {
                model: DirListStore.dirlistModel
                delegate: ListItem.Standard {
                    property var object: DirListStore.dirlistModel.get(index)
                    property bool isDir: object[FileObjectKey.keyIsdir]
                    property string fileName: AppUtility.fileObjectPathToFileName(object[FileObjectKey.keyPath])
                    property int category: object[FileObjectKey.keyCategory]
                    showDivider: true
                    iconName: isDir ? "file/folder" : Utility.categoryToIcon(category)
                    text: fileName
                    secondaryItem: Row {
                        height: childrenRect.height
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: Const.middleSpace
                        IconButton {
//                            color: Theme.lightDark(theme.primaryColor,
//                                                   Theme.dark.iconColor, Theme.dark.iconColor)
                            action: Action {
                                iconName: "file/file_download"
                            }
                        }
                        IconButton {
//                            color: Theme.lightDark(theme.primaryColor,
//                                                   Theme.dark.iconColor, Theme.dark.iconColor)
                            action: Action {
                                iconName: "social/share"
                            }
                        }
                        IconButton {
//                            color: Theme.lightDark(theme.primaryColor,
//                                                   Theme.dark.iconColor, Theme.dark.iconColor)
                            action: Action {
                                iconName: "navigation/more_vert"
                            }
                        }
                    }
                    onClicked: {
                        if (isDir) {
                            AppActions.showDir(object[FileObjectKey.keyPath]);
                        }
                    }
                }
            }
        }
    }
    Scrollbar {
        flickableItem: content
    }

}
