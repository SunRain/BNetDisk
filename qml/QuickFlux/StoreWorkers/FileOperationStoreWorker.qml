import QtQuick 2.0
import QuickFlux 1.0

import com.sunrain.bnetdisk.qmlplugin 1.0

import "../Stores"
import "../Actions"

StoreWorker {

    BDiskFileOperationDelegate {
        id: fileOperationDelegate
        onRequestFailure: {
            if (t == BDiskFileOperationDelegate.OPERATION_RENAME) {
                AppActions.snackbarInfo(qsTr("Rename file error!"));
            } else if (t == BDiskFileOperationDelegate.OPERATION_DELETE) {
                AppActions.snackbarInfo(qsTr("Delete file error!"));
            }
        }
        onRequestSuccess: {
            AppActions.snackbarInfo(qsTr("Operation success, but Baidu server may take some time to response. Please refresh manually if view not change."));
            delayRefreshDir.restart();
        }
    }

    // use a timer to delay refresh dirs to waiting for remote http server change internal data
    Timer {
        id: delayRefreshDir
        interval: 500
        repeat: false
        onTriggered: {
            AppActions.refreshCurrentView();
        }
    }

    Filter {
        type: ActionTypes.fileRename
        onDispatched: {
            var path = message.path
            var name = message.newName
            fileOperationDelegate.rename(path, name);
        }
    }

    Filter {
        type: ActionTypes.fileDelete
        onDispatched: {
            var path = message.path;
            fileOperationDelegate.deleteFile(path);
        }
    }

}
