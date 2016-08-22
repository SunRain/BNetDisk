pragma Singleton
import QtQuick 2.0
import QuickFlux 1.0
//import QSyncable 1.0

import com.sunrain.bnetdisk.qmlplugin 1.0

import "../Actions"

AppListener {
    id: searchStore

    property alias searchModel: searchDelegate.resultList

    property bool refreshing: false

    QtObject {
        id: inner
        property string key: ""
    }

    BDiskSearchDelegate {
        id: searchDelegate

        onStartRequest: {
            console.log("====== searchDelegate onStarted")
//            AppActions.showProgress();
            refreshing = true;
        }
        onFinishRequest: { //ret
            console.log("====== searchDelegate onFinished ")
//            AppActions.hideProgress();
            refreshing = false;
        }
        onRequestFailure: {
            AppActions.infomToNeedRelogin("Search error");
        }
    }

//    function clear() {
//        searchDelegate.clear();
//    }

//    function refresh() {
//        searchDelegate.search(inner.key);
//    }

    Filter {
        type: ActionTypes.search
        onDispatched: {
            inner.key = message.key;
            console.log("------------ search for "+inner.key);
            searchDelegate.search(inner.key);
        }
    }

    Filter {
        type: ActionTypes.refreshCurrentSearch
        onDispatched: {
            searchDelegate.search(inner.key);
        }
    }

    Filter {
        type: ActionTypes.clearCurrentSearch
        onDispatched: {
            searchDelegate.clear();
        }
    }
}
