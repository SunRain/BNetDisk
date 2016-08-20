import QtQuick 2.4
import Material 0.3
import QuickFlux 1.0

import "."
import "Pages"
import "QuickFlux/Actions"
import "QuickFlux/Adapters"

ApplicationWindow {
    id: bdiskWindow

    title: qsTr("BNetDisk")
    visible: true

    width: Const.screenWidth
    height: Const.screenHeight

    theme {
        primaryColor: "blue"
        accentColor: "red"
        tabHighlightColor: "white"
    }

    iconHelper {
        useQtResource: true
        /**************************
        if set useQtResource to true, alternativePath should be the prefix value in qrc
        if set useQtResource to false
            if using material system-wide icons, alternativePath should be setted to empty, Eg. ""
            if using application alternative icons, alternativePath should be setted to full path of icon path
        ***************************/
        alternativePath: "/"//iconHelper.applicationPath +"/image/icons"
    }

    Connections {
        target: LoginProvider
        onLogouted: {
            bdiskWindow.close();
        }
    }

    ProgressCircle {
        id: progressCircle
        anchors.centerIn: parent
        z: 10
        indeterminate: progressListener.showProgress
        width: dp(64)
        height: width
        dashThickness: dp(8)
    }
    Snackbar {
        id: dialogSnackBar
    }
    AppListener {
        id: progressListener
        property bool showProgress: false
        Filter {
            type: ActionTypes.showProgress
            onDispatched: {
                progressListener.showProgress = true;
            }
        }
        Filter {
            type: ActionTypes.hideProgress
            onDispatched: {
                progressListener.showProgress = false;
            }
        }
        Filter {
            type: ActionTypes.infomToNeedRelogin
            onDispatched: {
                var text = message.text;
                dialogSnackBar.open(text);
            }
        }
        Filter {
            type: ActionTypes.snackbarInfo
            onDispatched: {
                var info = message.info
                dialogSnackBar.open(info);
            }
        }
    }

    initialPage: MainViewPage {
        Component.onCompleted: {
            console.log("===== MainViewPage onCompleted")
            AppActions.showRootDir();
        }
    }

    // StoreAdapter is a utility to initial Store singleton component and
    // setup waitFor relationship. It is a workaround for QTBUG-49370
    StoreAdapter {

    }
}
