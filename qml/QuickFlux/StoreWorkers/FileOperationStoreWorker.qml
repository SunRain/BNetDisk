import QtQuick 2.0
import QuickFlux 1.0

import com.sunrain.bnetdisk.qmlplugin 1.0

import "../Stores"
import "../Actions"

StoreWorker {

    BDiskFileOperationDelegate {
        id: fileOperationDelegate
    }

    Filter {
        type: ActionTypes.fileRename
        onDispatched: {
            var path = message.path
            var name = message.newName
            console.log("====== onDispatched fileRename "+path+" name "+name);
        }
    }

}
