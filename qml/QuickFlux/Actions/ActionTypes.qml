pragma Singleton
import QtQuick 2.0
import QuickFlux 1.0

KeyTable {
    id: actionTypes

    property string showRootDir

//    property string selectMusicScannerDirs
    property string showProgress
    property string hideProgress
    property string showDir
    property string cdup
//    property string refreshCurrentDir
    property string refreshCurrentView
    property string infomToNeedRelogin

    property string snackbarInfo

    property string askToSelectDownloadPath
    property string downloadFile

    property string stopTask
    property string startTask

    property string showDownloadingComponent
    property string showCompletedComponent

    property string privShare
    property string pubShare

    property string askToRename
    property string fileRename

    property string askToDelete
    property string fileDelete

    property string showCategory

    property string askToOpenOverlayView

    property string showShareRecord

    property string askToCancelShare
    property string cancelShare

    property string showRecycleList
    property string askToRecycleRestore
    property string recycleRestore

    property string search

    property string refreshCurrentSearch
    property string clearCurrentSearch


//    property string openFileDialog


//    property string localMusicScannerFinish

//    property string showAlbumCategory
//    property string showArtistCategory
//    property string showGenresCategory
//    property string showAllMusic

//    property string adaptCtrlBarCover
//    property string changeCtrlBarCover

//    property string adaptCtrlBarTitle
//    property string changeCtrlBarTitle

//    property string adaptCtrlBarAlbumName
//    property string changeCtrlBarAlbumName

//    property string adaptCtrlBarArtistName
//    property string changeCtrlBarArtistName

//    property string adaptSkipBackward
//    property string doSkipBackward

//    property string adaptSkipForward
//    property string doSkipForward

//    property string changePlayMode

//    property string openPlstCreateDlg
//    property string playPlst
//    property string savePlst
//    property string openPlstModifyDlg
//    property string showTracksInPlst

//    property string openCategoryDetailView
}
