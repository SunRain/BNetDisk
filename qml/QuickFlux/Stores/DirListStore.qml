pragma Singleton
import QtQuick 2.0
import QuickFlux 1.0
import QSyncable 1.0

import com.sunrain.bnetdisk.qmlplugin 1.0

import "../Actions"
//import "../Adapters"

AppListener {
    id: dirListStore

    property alias dirlistModel: listDelegate

    DirListDelegate {
        id: listDelegate
        onStartRequest: {
            console.log("====== DirListDelegate onStarted")
            AppActions.showProgress();
        }
        onFinishRequest: {
            console.log("====== DirListDelegate onFinished")
            AppActions.hideProgress();
        }
    }

    Filter {
        type: ActionTypes.showRootDir
        onDispatched: {
            console.log("==== ActionTypes.showRootDir")
            listDelegate.showRoot();
        }
    }

}
