pragma Singleton
import QtQuick 2.0
import QuickFlux 1.0
import QSyncable 1.0

import com.sunrain.bnetdisk.qmlplugin 1.0

import "../Actions"

AppListener {
    id: searchStore

    property alias searchModel: searchDelegate.resultList

    BDiskSearchDelegate {
        id: searchDelegate

        onStartRequest: {
            console.log("====== searchDelegate onStarted")
            AppActions.showProgress();
        }
        onFinishRequest: { //ret
            console.log("====== searchDelegate onFinished ")
            AppActions.hideProgress();
        }
        onRequestFailure: {
            AppActions.infomToNeedRelogin("Search error");
        }
    }

    function clear() {
        searchDelegate.clear();
    }

    Filter {
        type: ActionTypes.search
        onDispatched: {
            var key = message.key;
            console.log("------------ search for "+key);
            searchDelegate.search(key);
        }
    }
}
