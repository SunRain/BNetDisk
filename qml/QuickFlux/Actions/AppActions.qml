pragma Singleton
import QtQuick 2.0
import QuickFlux 1.0

import "."

ActionCreator {
    signal showProgress();
    signal hideProgress();
//    signal selectMusicScannerDirs();
////    signal localMusicScannerFinish();

////    signal toMusicCategoryPage();
////    signal toAllMusicPage();

//    signal showAlbumCategory();
//    signal showArtistCategory();
//    signal showGenresCategory();
//    signal showAllMusic();

//    signal changePlayMode();

//    signal openPlstCreateDlg();

//    function togglePlayPause() {
//        Player.togglePlayPause();
//    }
//    function notifyLocalMusicScannerFinish() {
//        AppDispatcher.dispatch(ActionTypes.localMusicScannerFinish, "");
//    }
//    function changeCtrlBarCover(value) {
//        AppDispatcher.dispatch(ActionTypes.changeCtrlBarCover, {"value":value})
//    }
//    function changeCtrlBarTitle(value) {
//        AppDispatcher.dispatch(ActionTypes.changeCtrlBarTitle, {"value":value})
//    }
//    function changeCtrlBarAlbumName(value) {
//        AppDispatcher.dispatch(ActionTypes.changeCtrlBarAlbumName, {"value":value})
//    }
//    function changeCtrlBarArtistName(value) {
//        AppDispatcher.dispatch(ActionTypes.changeCtrlBarArtistName, {"value":value})
//    }

//    function savePlayList(name, musicObjectList, override) {
//        AppDispatcher.dispatch(ActionTypes.savePlst, {"name":name, "value":musicObjectList, "override":override})
//    }
//    function openPlstModifyDlg(name) {
//        AppDispatcher.dispatch(ActionTypes.openPlstModifyDlg, {"name":name})
//    }
//    function playPlst(name) {
//        AppDispatcher.dispatch(ActionTypes.playPlst, {"name":name})
//    }
//    function showTracksInPlst(name) {
//        AppDispatcher.dispatch(ActionTypes.showTracksInPlst, {"name":name});
//    }

//    function openCategoryDetailView(parent, hash, name, cover) {
//        AppDispatcher.dispatch(ActionTypes.openCategoryDetailView,
//                               {"viewParent":parent,
//                                   "hash":hash,
//                                   "name":name,
//                                   "cover":cover});
//    }

    signal showRootDir();
    signal cdup();
    signal refreshCurrentDir();
    signal showDownloadingComponent();
    signal showCompletedComponent();

    function showDir(dir) {
        AppDispatcher.dispatch(ActionTypes.showDir, {"dir":dir});
    }

    function infomToNeedRelogin(detail) {
        var text = qsTr("Need to relogin! Maybe this is due to ")+detail;
        AppDispatcher.dispatch(ActionTypes.infomToNeedRelogin, {"text":text});
    }

    function downloadFile(file, savePath, saveName) {
        console.log("===== dispatch downloadFile")
        AppDispatcher.dispatch(ActionTypes.downloadFile,
                               {
                                   "file":file,
                                   "savePath":savePath,
                                   "saveName":saveName
                               });
    }

    function askToSelectDownloadPath(file, saveName) {
        AppDispatcher.dispatch(ActionTypes.askToSelectDownloadPath,
                               {
                                   "file":file,
                                   "saveName":saveName
                               });
    }

    function showDownloadPage(stack) {
        stack.push(Qt.resolvedUrl("../../Pages/DownloadViewPage.qml"))
    }

    function stopTask(hash) {
        AppDispatcher.dispatch(ActionTypes.stopTask, {"hash":hash});
    }
    function startTask(hash) {
        AppDispatcher.dispatch(ActionTypes.startTask, {"hash":hash});
    }

    function privShare(fsId) {
        AppDispatcher.dispatch(ActionTypes.privShare, {"fsid":fsId});
    }
    function pubShare(fsId) {
        AppDispatcher.dispatch(ActionTypes.pubShare, {"fsid":fsId});
    }
}
