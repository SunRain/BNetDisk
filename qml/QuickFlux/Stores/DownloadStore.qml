pragma Singleton
import QtQuick 2.0
import QuickFlux 1.0

import com.sunrain.bnetdisk.qmlplugin 1.0

import "../Actions"

AppListener {
    id: downloadStore

    property string  bibibi

    BDiskDownloadDelegate {
        id: dlDelegate
    }

    Filter {
        type: ActionTypes.downloadFile
        onDispatched: {
            console.log("==== onDispatched downloadFile")
            //TODO unuse param atm
            dlDelegate.download();
        }
    }
}
