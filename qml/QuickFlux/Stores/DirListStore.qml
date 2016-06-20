pragma Singleton
import QtQuick 2.0
import QuickFlux 1.0
import QSyncable 1.0

import com.sunrain.bnetdisk.qmlplugin 1.0

import "../Actions"
//import "../Adapters"

AppListener {
    id: dirListStore

    property alias dirlistModel: listDelegate.dirList
    property alias currentPath: listDelegate.currentPath

    DirListDelegate {
        id: listDelegate
        onStartRequest: {
            console.log("====== DirListDelegate onStarted")
            AppActions.showProgress();
        }
        onFinishRequest: { //ret
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
    Filter {
        type: ActionTypes.cdup
        onDispatched: {
            console.log("=== onDispatched cdup")
            listDelegate.cdup();
        }
    }
    Filter {
        type: ActionTypes.refreshCurrentDir
        onDispatched: {
            listDelegate.refresh();
        }
    }
    Filter {
        type: ActionTypes.showDir
        onDispatched: {
            var dir = message.dir
            listDelegate.show(dir);
        }
    }

}
