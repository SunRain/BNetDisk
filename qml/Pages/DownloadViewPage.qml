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
                onClicked: {
                    AppActions.showDownloadingComponent();
                }
            }
            ListItem.Standard {
                text: qsTr("Uploading")
            }
            ListItem.Standard {
                text: qsTr("Finished")
                onClicked: {
                    AppActions.showCompletedComponent();
                }
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
        source: DownloadStore.downloadPageviewComponentUri;
    }


}
