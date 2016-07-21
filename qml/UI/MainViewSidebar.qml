import QtQuick 2.4
import Material 0.3
import Material.ListItems 0.1 as ListItem

import com.sunrain.bnetdisk.qmlplugin 1.0

import ".."
import "../QuickFlux/Stores"
import "../QuickFlux/Actions"
import "../QuickFlux/Scripts"

Sidebar {
    width: Const.sidebarWidth

    property var nameList: [
        qsTr("All"), qsTr("Image"), qsTr("Document"), qsTr("Video"),
        qsTr("BT"), qsTr("Audio"), qsTr("Other")
    ]

    property var iconList: [
        "file/attachment", "image/image", "image/texture", "av/movie",
        "file/attachment", "image/music_note", "file/attachment"
    ]

    Column {
        width: parent.width
        ListItem.Subheader {
            text: qsTr("Cloud Storage")
        }
        Repeater {
            model: sidebar.nameList.length
            delegate: ListItem.Standard {
                text: sidebar.nameList[index]
                iconName: sidebar.iconList[index]
            }
        }
        ListItem.Divider{}
        ListItem.Standard {
            action: IconButton {
                anchors {
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                }
                action: Action {
                    iconName: "social/share"
                    onTriggered: {
                        console.log("===== share icon click")
                    }
                }
            }
            content: IconButton {
                anchors {
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                }
                action: Action {
                    iconName: "action/delete"
                    onTriggered: {
                        console.log("===== trash icon click")
                    }
                }
            }
        }
    }
}
