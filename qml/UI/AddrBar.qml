import QtQuick 2.4
import Material 0.3
import Material.ListItems 0.1 as ListItem

import com.sunrain.bnetdisk.qmlplugin 1.0

import ".."
import "../QuickFlux/Stores"
import "../QuickFlux/Actions"
import "../QuickFlux/Scripts"

Item {
    id: addrBar
    width: parent ? parent.width : Const.screenWidth
    height: parent ? parent.height : Const.screenHeight

    property bool showRefreshAction: false
    property bool clickable: true

    IconButton {
        id: refreshBtn
        anchors {
            left: parent.left
            verticalCenter: parent.verticalCenter
        }
        color: Theme.lightDark(theme.primaryColor, Theme.light.iconColor, Theme.dark.iconColor)
        action: Action {
            iconName: "navigation/refresh"
            onTriggered: {
                AppActions.refreshCurrentDir();
            }
        }
        visible: showRefreshAction
        enabled: visible
    }

    ListView {
        id: pathView
        anchors {
            left: showRefreshAction ? refreshBtn.right :  parent.left
            leftMargin: showRefreshAction ? Const.middleSpace : 0
            right: parent.right
            rightMargin: showRefreshAction ? Const.middleSpace : 0
        }
        height: parent.height > Const.itemHeight ? Const.itemHeight : parent.height
        boundsBehavior: Flickable.OvershootBounds
        clip: true
        orientation: Qt.Horizontal
        spacing: Const.tinySpace
        onCountChanged: {
            pathView.positionViewAtEnd();
        }
        onWidthChanged: {
            pathView.positionViewAtEnd();
        }
        model: DirListStore.currentPathList
        delegate: ListItem.BaseListItem {
            id: pathItem
            height: parent.height
            /// NOTE anchors.left && anchors.right should set before width property,
            /// This fix qml-material View component bug
            anchors.left: undefined
            anchors.right: undefined
            width: itemRow.width
            interactive: clickable
            onClicked: {
                if (index == 0) {
                    AppActions.showDir("/");
                } else {
                    var dir = "";
                    for(var i=1; i<=index; ++i) {
                        dir += "/";
                        dir += DirListStore.currentPathList[i];
                    }
                    AppActions.showDir(dir);
                }
            }
            Row {
                id: itemRow
                height: parent.height
                spacing: Const.tinySpace
                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    style: "subheading"
                    color:  Theme.isDarkColor(actionBar.backgroundColor)
                            ? Theme.dark.textColor
                            : Theme.light.accentColor
                    text: index == 0 ? (showRefreshAction ? qsTr("BNetDisk") : qsTr("RootDir")) : DirListStore.currentPathList[index]
                    verticalAlignment: Text.AlignVCenter
                }
                Icon {
                    anchors.verticalCenter: parent.verticalCenter
                    color: Theme.isDarkColor(actionBar.backgroundColor)
                           ? Theme.dark.iconColor
                           : Theme.light.accentColor
                    name: "navigation/chevron_right"
                    visible: index != DirListStore.currentPathList.length -1
                }
            }
        }
    }
    Scrollbar {
        flickableItem: pathView
    }
}
