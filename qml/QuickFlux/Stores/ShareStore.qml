pragma Singleton
import QtQuick 2.0
import QuickFlux 1.0
import QSyncable 1.0

import com.sunrain.bnetdisk.qmlplugin 1.0

import "../Actions"

AppListener {
    id: shareStore

    property string sharelink: ""
    property string password: ""
    property bool showErrorLabel: false

    BDiskShareDelegate {
        id: shareDelegate
        onRequestFailure: {
            showErrorLabel = true;
        }
        onPubShareLink: {
            sharelink = url;
        }
        onPrivShareLink: {
            sharelink = url;
            password = passwd;
        }
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
    function clear() {
        sharelink = "";
        password = "";
        showErrorLabel = false;
    }
}
