import QtQuick 2.4
import Material 0.3
import Material.ListItems 0.1 as ListItem

import com.sunrain.bnetdisk.qmlplugin 1.0

import "../QuickFlux/Actions"
import "../QuickFlux/Scripts"
import "../QuickFlux/Stores"
import "../"

ListView {
    id: compltedTaskView
    anchors.fill: parent
    clip: true
    model: DownloadStore.completedModel
    delegate: ListItem.Standard {
        id: dlItem
        property var object: DownloadStore.completedModel[index]
        property var uuid: object[DownloaderObjectKey.KeyIdentifier]
        property string path: object[DownloaderObjectKey.keyFilePath]
        property string fileName: AppUtility.fileObjectPathToFileName(path)
        property int totalSize: object[DownloaderObjectKey.keyTotalSize];
        property int readySize: object[DownloaderObjectKey.keyReadySize];
        property string speed: AppUtility.bytesPerSecond(0);
        property string percent: AppUtility.downloadPercent(readySize, totalSize);
        property string elapsedTime: AppUtility.milliSecsToStr(0)
        //                property bool taskRunning: DownloadStore.taskRunning(uuid)
        width: parent.width
        //                height: dlItemColumn.height + Const.middleSpace
        showDivider: true
        //                Component.onCompleted: {
        //                    console.log('.......................................')
        //                    for (var prop in object) {
        //                        console.log("Object item:", prop, "=", object[prop])
        //                    }
        //                    console.log("==== uuid ["+uuid+"] object ["+object+"]");
        //                    console.log("========== BDiskEvent.STATUS_RUNNING "+BDiskEvent.STATUS_RUNNING
        //                                + "   BDiskEvent.STATUS_STOP "+BDiskEvent.STATUS_STOP)
        //                }
        text: fileName
    }
}
