import QtQuick 2.0
import QuickFlux 1.0

import com.sunrain.bnetdisk.qmlplugin 1.0

import "../Stores"
import "../Actions"

StoreWorker {
    id: shareStoreWorker

    BDiskShareDelegate {
        id: shareDelegate
        onRequestFailure: {
            ShareStore.showErrorLabel = true;
        }
        onPubShareLink: {
            ShareStore.sharelink = url;
        }
        onPrivShareLink: {
            ShareStore.sharelink = url;
            ShareStore.password = passwd;
        }
        onShareRecordsChanged: {
            ShareStore.shareRecordList = shareRecords;
        }
        onCancelShareSuccess: {
            console.log("========== onCancelShareSuccess");
            AppActions.showShareRecord(1);
        }
    }

    function clear() {
        ShareStore.sharelink = "";
        ShareStore.password = "";
        ShareStore.showErrorLabel = false;
    }

    Filter {
        type: ActionTypes.privShare
        onDispatched: {
            clear();
            var id = message.fsid;
            console.log('=================== onDispatched privShare ' + id);
            shareDelegate.privShare(id);
        }
    }
    Filter {
        type: ActionTypes.pubShare
        onDispatched: {
            clear();
            var id = message.fsid;
            console.log('=================== onDispatched pubShare ' + id);
            shareDelegate.pubShare(id);
        }
    }

    Filter {
        type: ActionTypes.showShareRecord
        onDispatched: {
            var page = message.page;
            console.log('=================== onDispatched showShareRecord ' + page);
            shareDelegate.showShareRecord(page);
        }
    }

    Filter {
        type: ActionTypes.cancelShare
        onDispatched: {
            var shareId = message.shareId;
            shareDelegate.cancelShare(shareId);
        }
    }
}
