import QtQuick 2.0
import QuickFlux 1.0
import QtQuick.Dialogs 1.2
import Material 0.3

import com.sunrain.bnetdisk.qmlplugin 1.0

import "../Actions"
import "../Stores"

Item {

    Dialog {
        id: rename
        title: qsTr("Rename")
        hasActions: true
        property alias name: renameText.text
        TextField {
            id: renameText
            width: parent.width
        }
    }

    Dialog {
        id: info
        hasActions: true
    }

    AppScript {
        runWhen: ActionTypes.askToRename
        script: {
            var path = message.path;
            var file = AppUtility.fileObjectPathToFileName(path);
            console.log("======= path "+path
                        + "  file "+file);
            rename.name = file;
            rename.open();
            once(rename.onAccepted, function() {
                console.log("===== rename on onAccepted, new name "+rename.name);
                AppActions.fileRename(path, rename.name);
            });
            once(rename.onRejected, exit.bind(this, 0));
        }
    }

    AppScript {
        runWhen: ActionTypes.askToDelete
        script: {
            var path = message.path;
            info.text = qsTr("Confirm to delete")+" ["+path+"] ?";
            info.open();
            once(info.onAccepted, function() {
                console.log("===== delete "+path);
                AppActions.fileDelete(path)
            });
            once(info.onRejected, exit.bind(this, 0));
        }
    }

    AppScript {
        runWhen: ActionTypes.askToCancelShare
        script: {
            var shareId = message.shareId;
            info.title = qsTr("Cancel Share?");
            info.text = qsTr("Canceling share will delete this share link and prevent people visit file from this link");
            info.open();
            once(info.onAccepted, function() {
                AppActions.cancelShare(shareId);
            });
            once(info.onRejected, exit.bind(this, 0));
        }
    }

    AppScript {
        runWhen: ActionTypes.askToRecycleRestore
        script: {
            var id = message.fsId;
            info.text = qsTr("Restore current file?");
            info.open();
            once(info.onAccepted, function() {
                AppActions.recycleRestore(id)
            });
            once(info.onRejected, exit.bind(this, 0));
        }
    }

    AppScript {
        runWhen: ActionTypes.askToLogout
        script: {
            info.text = qsTr("Logout now?");
            info.open();
            once(info.onAccepted, function() {
                LoginProvider.logout();
            });
            once(info.onRejected, exit.bind(this, 0));
        }
    }


}
