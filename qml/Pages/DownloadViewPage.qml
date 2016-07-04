import QtQuick 2.4
import Material 0.3
import Material.ListItems 0.1 as ListItem

import com.sunrain.bnetdisk.qmlplugin 1.0

import ".."
import "../QuickFlux/Stores"
import "../QuickFlux/Actions"
import "../QuickFlux/Scripts"

Page {
    id: dlViewPage


    Sidebar {
        id: sidebar
        Column {
            width: parent.width
            ListItem.Standard {
                text: qsTr("Downloading")
            }
            ListItem.Standard {
                text: qsTr("Uploading")
            }
            ListItem.Standard {
                text: qsTr("Finished")
            }
        }
    }
    Loader {
        id: viewLoader
        anchors {
            top: parent.top
            topMargin: Const.tinySpace
            left: sidebar.right
            leftMargin: Const.tinySpace
            right: parent.right
            rightMargin: Const.tinySpace
            bottom: parent.bottom
        }
        sourceComponent: dlComponent
    }








    Component {
        id: dlComponent
        ListView {
            anchors.fill: parent
            clip: true
//            spacing: dp(8)
            model: DownloadStore.downloadingModel
            delegate: ListItem.BaseListItem {
                id: dlItem
                property var object: DownloadStore.downloadingModel[index]
                property var hash: object[DownloaderObjectKey.KeyIdentifier]
                property string path: object[DownloaderObjectKey.keyFilePath]
                property string fileName: AppUtility.fileObjectPathToFileName(path)
                property int totalSize: object[DownloaderObjectKey.keyTotalSize];
                property int readySize: object[DownloaderObjectKey.keyReadySize];
                width: parent.width
                height: dlItemColumn.height + Const.middleSpace
                showDivider: true
                Component.onCompleted: {
                    for (var prop in object) {
                        console.log("Object item:", prop, "=", topobject[prop])
                    }
                    console.log("==== hash ["+hash+"] object ["+object+"]");
                }

                IconButton {
                    id: dlItemIcon
                    size: parent.height * 0.5
                    anchors {
                        left: parent.left
                        verticalCenter: parent.verticalCenter
                    }
                    action: Action {
                        iconName: "av/play_circle_filled"
                    }
                }
                Column {
                    id: dlItemColumn
                    anchors {
                        left: dlItemIcon.right
                        leftMargin: Const.middleSpace
                        right: parent.right
                        rightMargin: Const.middleSpace
                        verticalCenter: parent.verticalCenter
                    }
                    spacing: Const.tinySpace
                    Label {
                        width: parent.width
                        elide: Text.ElideRight
                        style: "subheading"
                        text: fileName
                    }
                    ProgressBar {
                        width: parent.width
                        color: theme.accentColor
                        maximumValue: 100
                        minimumValue: 0
                        value: 50
                        NumberAnimation on value {
                            to: value
                            duration: 100
                        }
                    }
                    Label {
                        width: parent.width
                        color: Theme.light.subTextColor
                        elide: Text.ElideRight
                        wrapMode: Text.WordWrap
                        style: "body1"
                        text: "downloading info"
                    }
                    Label {
                        width: parent.width
                        color: Theme.light.subTextColor
                        elide: Text.ElideRight
                        wrapMode: Text.WordWrap
                        style: "body1"
                        text: "downloading info2"
                    }
                }
            }
        }
    }




}
