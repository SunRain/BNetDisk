pragma Singleton
import QtQuick 2.0
import QuickFlux 1.0
//import QSyncable 1.0

import com.sunrain.bnetdisk.qmlplugin 1.0

import "../Actions"
//import "../Adapters"

AppListener {
    id: dirListStore

    property alias dirlistModel: listDelegate.dirList
    property alias currentPath: listDelegate.currentPath
    property alias currentPathList: listDelegate.currentPathList

    property bool showGridView: false

    /// indicate current show category or all
    property bool viewShowAllState: inner.viewType == inner.viewCategoryAll

    property bool refreshing: false

    QtObject {
        id: inner
        readonly property int viewCategoryAll: 0
        readonly property int viewCategoryVideo: 1
        readonly property int viewCategoryImage: 2
        readonly property int viewCategoryDoc: 3
        readonly property int viewCategoryExe: 4
        readonly property int viewCategoryBT: 5
        readonly property int viewCategoryMusic: 6
        readonly property int viewCategoryOther: 7
        readonly property int viewRecycleList: 8

        property int viewType: viewCategoryAll

        property int page: 1
    }

    DirListDelegate {
        id: listDelegate
        onStartRequest: {
            console.log("====== DirListDelegate onStarted")
//            AppActions.showProgress();
            refreshing = true;
        }
        onFinishRequest: { //ret
            console.log("====== DirListDelegate onFinished")
//            AppActions.hideProgress();
            refreshing = false;
        }
        onRequestFailure: {
            AppActions.infomToNeedRelogin("get dirlist error");
        }
        onRecycleRestoreSuccess: {
            AppActions.showRecycleList(inner.page);
        }
    }

    Filter {
        type: ActionTypes.showRootDir
        onDispatched: {
            inner.viewType = inner.viewCategoryAll;
            listDelegate.showRoot();
        }
    }
    Filter {
        type: ActionTypes.cdup
        onDispatched: {
            inner.viewType = inner.viewCategoryAll;
            listDelegate.cdup();
        }
    }
//    Filter {
//        type: ActionTypes.refreshCurrentDir
//        onDispatched: {
//            listDelegate.refresh();
//        }
//    }
    Filter {
        type: ActionTypes.refreshCurrentView
        onDispatched: {
            if (inner.viewType == inner.viewCategoryAll) {
                listDelegate.refresh();
            } else if (inner.viewType == inner.viewCategoryBT) {
                listDelegate.showBT(inner.page);
            } else if (inner.viewType == inner.viewCategoryDoc) {
                listDelegate.showDoc(inner.page);
            } else if (inner.viewType == inner.viewCategoryExe) {
                listDelegate.showExe(inner.page);
            } else if (inner.viewType == inner.viewCategoryImage) {
                listDelegate.showImage(inner.page);
            } else if (inner.viewType == inner.viewCategoryMusic) {
                listDelegate.showMusic(inner.page);
            } else if (inner.viewType == inner.viewCategoryOther) {
                listDelegate.showOther(inner.page);
            } else if (inner.viewType == inner.viewCategoryVideo) {
                listDelegate.showVideo(inner.page);
            } else if (inner.viewType == inner.viewRecycleList) {
                listDelegate.showRecycleList(inner.page);
            }
        }
    }

    Filter {
        type: ActionTypes.showDir
        onDispatched: {
            inner.viewType = inner.viewCategoryAll;
            var dir = message.dir;
            listDelegate.showDir(dir);
        }
    }
    Filter {
        type: ActionTypes.showCategory
        onDispatched: {
            var category = message.category;
            if (category != "all") {
                inner.page = message.page;
            }

            if (category == "image" || category == "video") {
                dirListStore.showGridView = true;
            } else {
                dirListStore.showGridView = false;
            }

            if (category == "video") {
                inner.viewType = inner.viewCategoryVideo;
                listDelegate.showVideo(inner.page);
            } else if (category == "image") {
                inner.viewType = inner.viewCategoryImage;
                listDelegate.showImage(inner.page);
            } else if (category == "doc") {
                inner.viewType = inner.viewCategoryDoc;
                listDelegate.showDoc(inner.page);
            } else if (category == "exe") {
                inner.viewType = inner.viewCategoryExe;
                listDelegate.showExe(inner.page);
            } else if (category == "bt") {
                inner.viewType = inner.viewCategoryBT
                listDelegate.showBT(inner.page);
            } else if (category == "music") {
                inner.viewType = inner.viewCategoryMusic;
                listDelegate.showMusic(inner.page);
            } else if (category == "other") {
                inner.viewType = inner.viewCategoryOther;
                listDelegate.showOther(inner.page);
            } else if (category == "all") {
                inner.viewType = inner.viewCategoryAll;
                listDelegate.refresh();
            }
        }
    }

    Filter {
        type: ActionTypes.showRecycleList
        onDispatched: {
            inner.viewType = inner.viewRecycleList;

            inner.page = message.page;

            dirListStore.showGridView = false;

            listDelegate.showRecycleList(inner.page);
        }
    }

    Filter {
        type: ActionTypes.recycleRestore
        onDispatched: {
            var fsid = message.fsId;
            listDelegate.recycleRestore(fsid);
        }
    }
}
