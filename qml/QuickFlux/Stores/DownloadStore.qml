pragma Singleton
import QtQuick 2.0
import QuickFlux 1.0

import com.sunrain.bnetdisk.qmlplugin 1.0

import "../Actions"

AppListener {
    id: downloadStore

    property alias downloadingModel: dlDelegate.tasks

    BDiskDownloadDelegate {
        id: dlDelegate
    }

    Filter {
        type: ActionTypes.downloadFile
        onDispatched: {
            //TODO unuse param atm
//            dlDelegate.download();
            var file = message.file;
            var saveName = message.saveName;
            var savePath = message.savePath;
            console.log("==== onDispatched downloadFile "+file+ " save to "+savePath+ " "+saveName);
            dlDelegate.download(file, savePath, saveName);
        }
    }
}
