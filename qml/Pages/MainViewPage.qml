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
//                musicLibraryManager.scanLocalMusic();
//                AppActions.selectMusicScannerDirs();
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

    ListItemContent {
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
        onShareMenuClicked: {
            shareMenu.shareId = id;
            shareMenu.parent = parentItem;
            shareMenu.open(parentItem, 0, 0);

        }
        onMoreVertMenuClicked: {
            moreVertMenu.path = path;
            moreVertMenu.parent = parentItem;
            moreVertMenu.open(parentItem, 0, 0);
        }
    }

    Scrollbar {
        flickableItem: content
    }

    Dropdown {
        id: shareMenu
        anchor: Item.TopRight
        width: mainViewPage.width /4
        height: options.height
        property string shareId: ""

        Rectangle {
            anchors.fill: parent
            radius: 2 * Units.dp
        }
        Column {
            id: options
            width: parent.width
            ListItem.Standard {
                text: qsTr("share public")
                onClicked: {
                    shareLinkView.open(mainViewPage);
                    AppActions.pubShare(shareMenu.shareId);
                    shareMenu.close();
                }
            }
            ListItem.Standard {
                text: qsTr("share private")
                onClicked: {
                    shareLinkView.open(mainViewPage);
                    AppActions.privShare(shareMenu.shareId);
                    shareMenu.close();
                }
            }
        }
    }

    Dropdown {
        id: moreVertMenu
        anchor: Item.TopRight
        width: mainViewPage.width /4
        height: moreVertColumn.height

        property string path: ""

        Rectangle {
            anchors.fill: parent
            radius: 2 * Units.dp
        }
        Column {
            id: moreVertColumn
            width: parent.width
            ListItem.Standard {
                text: qsTr("rename")
                onClicked: {
                    AppActions.askToRename(moreVertMenu.path);
                    moreVertMenu.close();
                }
            }
            ListItem.Standard {
                text: qsTr("delete")
                onClicked: {
                    AppActions.askToDelete(moreVertMenu.path);
                    moreVertMenu.close();
                }
            }
        }
    }

    OverlayView {
        id: shareLinkView
        width: Const.screenWidth * 0.6
        height: shareLinkContent.height + Const.middleSpace * 2
        anchors.centerIn: parent
        Column {
            id: shareLinkContent
            width: parent.width - Const.middleSpace * 2
            anchors.centerIn: parent
            spacing: Const.middleSpace * 2
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
        }
    }
}
