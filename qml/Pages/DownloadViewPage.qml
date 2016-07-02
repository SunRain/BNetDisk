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
            spacing: dp(8)
            model: DownloadStore.downloadingModel
            delegate: ListItem.Standard {
                id: dlItem
                property var object: DownloadStore.downloadingModel[index]
                property string path: object[DownloaderObjectKey.keyFilePath]
                property string fileName: AppUtility.fileObjectPathToFileName(path)
                showDivider: true
                property int totalSize: object[DownloaderObjectKey.keyTotalSize];
                property int readySize: object[DownloaderObjectKey.keyReadySize];
                text: fileName + " "+totalSize +" "+readySize
            }
        }
    }




}
