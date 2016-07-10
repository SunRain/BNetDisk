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
                property var uuid: object[DownloaderObjectKey.KeyIdentifier]
                property string path: object[DownloaderObjectKey.keyFilePath]
                property string fileName: AppUtility.fileObjectPathToFileName(path)
                property int totalSize: object[DownloaderObjectKey.keyTotalSize];
                property int readySize: object[DownloaderObjectKey.keyReadySize];
                property string speed: AppUtility.bytesPerSecond(0);
                property string percent: AppUtility.downloadPercent(readySize, totalSize);
                property string elapsedTime: AppUtility.milliSecsToStr(0)
                property bool taskRunning: DownloadStore.taskRunning(uuid)
                width: parent.width
                height: dlItemColumn.height + Const.middleSpace
                showDivider: true
                Component.onCompleted: {
                    console.log('.......................................')
                    for (var prop in object) {
                        console.log("Object item:", prop, "=", object[prop])
                    }
                    console.log("==== uuid ["+uuid+"] object ["+object+"]");
                    console.log("========== BDiskEvent.STATUS_RUNNING "+BDiskEvent.STATUS_RUNNING
                                + "   BDiskEvent.STATUS_STOP "+BDiskEvent.STATUS_STOP)
                }
                Connections {
                    target: DiskEvent
                    onDownloadProgress: { //hash, int bytesPerSecond, int bytesDownloaded, qint64 runElapsedMSecs
                        if (hash == uuid) {
                            dlItem.percent = AppUtility.downloadPercent(bytesDownloaded, totalSize);
                            dlItem.readySize = bytesDownloaded;
                            dlItem.speed = AppUtility.bytesPerSecond(bytesPerSecond);
                            dlItem.elapsedTime = AppUtility.milliSecsToStr(runElapsedMSecs);
                        }
                    }
                    onTaskStatusChanged: { //const QString &hash, BDiskEvent::TaskStatus status)
                        if (hash == uuid) {
                            console.log('======== onTaskStatusChanged '+status);
                            if (status == BDiskEvent.STATUS_RUNNING) {
                                dlItem.taskRunning = true;
                            } else {
                                dlItem.taskRunning = false;
                            }
                        }
                    }
                }

                IconButton {
                    id: dlItemIcon
                    size: parent.height * 0.5
                    anchors {
                        left: parent.left
                        verticalCenter: parent.verticalCenter
                    }
                    action: Action {
                        iconName: dlItem.taskRunning ? "av/pause_circle_filled" : "av/play_circle_filled"
                    }
                    onClicked: {
                        if (dlItem.taskRunning) {
                            AppActions.stopTask(dlItem.uuid);
                        } else {
                            AppActions.startTask(dlItem.uuid);
                        }
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
                        id: dlProgressBar
                        width: parent.width
                        color: theme.accentColor
                        maximumValue: 100
                        minimumValue: 0
                        value: dlItem.readySize/dlItem.totalSize *100
                        NumberAnimation on value {
                            to: dlProgressBar.value
                            duration: 100
                        }
                    }
                    Label {
                        width: parent.width
                        color: Theme.light.subTextColor
                        elide: Text.ElideRight
                        wrapMode: Text.WordWrap
                        style: "body1"
                        text: dlItem.taskRunning
                              ? qsTr("Downloading")+" - "+dlItem.percent+" - "+dlItem.elapsedTime
                              : qsTr("Paused")+" · "+dlItem.percent
                    }
                    Label {
                        width: parent.width
                        color: Theme.light.subTextColor
                        elide: Text.ElideRight
                        wrapMode: Text.WordWrap
                        style: "body1"
                        text: AppUtility.sizeToStr(dlItem.readySize)
                              +"/"
                              +AppUtility.sizeToStr(dlItem.totalSize)
                              +" - "
                              +dlItem.speed+" ↓"
                    }
                }
            }
        }
    }




}
