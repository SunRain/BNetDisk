import QtQuick 2.0
import QuickFlux 1.0
import QtQuick.Dialogs 1.2

import "../Actions"
import "../Stores"

Item {
    FileDialog {
        id: dialog
        title: qsTr("Select save path")
        selectFolder: true
        folder: shortcuts.home
    }

    AppScript {
        runWhen: ActionTypes.askToSelectDownloadPath
        script: {
            var file = message.file;
            var saveName = message.saveName;
            dialog.open()
            once(dialog.onAccepted, function(){
                var folder = dialog.fileUrl;
                AppActions.downloadFile(file, folder, saveName);
            });
            once(dialog.onRejected, exit.bind(this, 0));
            once(ActionTypes.downloadFile, exit.bind(this, 0));
        }
    }
}
