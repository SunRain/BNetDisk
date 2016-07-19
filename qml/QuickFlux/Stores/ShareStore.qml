pragma Singleton
import QtQuick 2.0
import QuickFlux 1.0
import QSyncable 1.0

import com.sunrain.bnetdisk.qmlplugin 1.0

import "../Actions"

AppListener {
    id: shareStore

    function privShare(id) {
        shareDelegate.privShare(id);
    }

    function pubShare(id) {
        shareDelegate.pubShare(id);
    }

    BDiskShareDelegate {
        id: shareDelegate
    }

}
