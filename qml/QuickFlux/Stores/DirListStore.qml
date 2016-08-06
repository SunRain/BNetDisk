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
    property alias currentPathList: listDelegate.currentPathList

    property bool showGridView: false

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
        onRequestFailure: {
            AppActions.infomToNeedRelogin("get dirlist error");
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
    Filter {
        type: ActionTypes.showCategory
        onDispatched: {
            var category = message.category;
            var page = message.page;

            if (category == "image" || category == "video") {
                dirListStore.showGridView = true;
            } else {
                dirListStore.showGridView = false;
            }

            if (category == "video") {
                listDelegate.showVideo(page);
            } else if (category == "image") {
                listDelegate.showImage(page);
            } else if (category == "doc") {
                listDelegate.showDoc(page);
            } else if (category == "exe") {
                listDelegate.showExe(page);
            } else if (category == "bt") {
                listDelegate.showBT(page);
            } else if (category == "music") {
                listDelegate.showMusic(page);
            } else if (category == "other") {
                listDelegate.showOther(page);
            } else if (category == "all") {
                listDelegate.refresh();
            }
        }
    }
}
