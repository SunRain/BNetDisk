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
    id: mainViewPage

    actions: [
        Action {
            iconName: "action/search"
            name: qsTr("Search")
            onTriggered: {
                AppActions.showSearchPage(pageStack);
            }
        },
        Action {
            iconName: "action/account_box"
            name: qsTr("Logout")
            onTriggered: {
                LoginProvider.logout();
            }
        },
        Action {
            iconName: "file/cloud_download"
            name: qsTr("Transmission")
            onTriggered: {
//                pageStack.push(Qt.resolvedUrl("DownloadViewPage.qml"))
                AppActions.showDownloadPage(pageStack);
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

    actionBar.customContent: AddrBar {}

    FileSavePathDialog {}

    MainViewSidebar {
        id: sidebar
        anchors.bottom: infoBanner.top
    }

    DiskOperationDialog {
        id: opDialog
    }

    View {
        id: infoBanner
        width: sidebar.width
        height: infoColumn.height
        anchors {
            left: sidebar.left//parent.left
            bottom: parent.bottom
        }
        backgroundColor: style === "default" ? "white" : "#333"
        Rectangle {
            width: 1
            height: parent.height
            anchors.right: parent.right
            color: style === "dark" ? Qt.rgba(0.5,0.5,0.5,0.5) : Theme.light.dividerColor
        }
        Column {
            id: infoColumn
            width: parent.width
            Rectangle {
                width: parent.width
                height: 1
                color: style === "dark" ? Qt.rgba(0.5,0.5,0.5,0.5) : Theme.light.dividerColor
            }
            ListItem.Standard {
                width: parent.width
                text: DownloadStore.downloadingModel.length + " " + qsTr("running task");
                onClicked: {
                    AppActions.showDownloadPage(pageStack);
                }
            }
        }
    }

    Loader {
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
        sourceComponent: sidebar.displayType == sidebar.displayTypeCategoryView
        ? (DirListStore.showGridView ? gridItemComponent : listItemComponent)
        : (sidebar.displayType == sidebar.displayTypeShareRecord
           ? shareRecordView
           : (DirListStore.showGridView ? gridItemComponent : listItemComponent)
          )
    }

    Component {
        id: shareRecordView
        ShareRecordView {
            anchors.fill: parent
        }
    }

    Component {
        id: listItemComponent
        ListItemContent {
            anchors.fill: parent
            onShareMenuClicked: {
                dropDownMenu.shareId = id;
                dropDownMenu.parent = parentItem;
                dropDownMenu.showDelete = false;
                dropDownMenu.showRename = false;
                dropDownMenu.showDelete = false;
                dropDownMenu.showShare = true;
                dropDownMenu.open(parentItem, 0, 0);

            }
            onMoreVertMenuClicked: {
                dropDownMenu.path = path;
                dropDownMenu.parent = parentItem;
                dropDownMenu.showDownload = false;
                dropDownMenu.showShare = false;
                dropDownMenu.showDelete = true;
                dropDownMenu.showRename = true;
                dropDownMenu.open(parentItem, 0, 0);
            }
        }
    }

    Component {
        id: gridItemComponent
        GridItemContent {
            onMenuClicked: {
                dropDownMenu.path = path;
                dropDownMenu.fileName = name;
                dropDownMenu.parent = parentItem;
                dropDownMenu.shareId = fsid;
                dropDownMenu.showDownload = true;
                dropDownMenu.showShare = true;
                dropDownMenu.showDelete = true;
                dropDownMenu.showRename = true;
                dropDownMenu.open(parentItem, 0, 0);
            }
            onImageClicked: {
                AppActions.openImagePreviewOverlayView(index);
                overlayView.open(mainViewPage);
            }
        }
    }

    Dropdown {
        id: dropDownMenu
        anchor: Item.TopRight
        width: mainViewPage.width /4
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

    OverlayView {
        id: overlayView
        width: Const.screenWidth * 0.8
        height: overlayViewLoader.height
        Loader {
            id: overlayViewLoader
            width: parent.width
            source: OverlayViewStore.overlayViewUri
        }
    }
}
