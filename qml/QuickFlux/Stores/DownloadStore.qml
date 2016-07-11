pragma Singleton
import QtQuick 2.0
import QuickFlux 1.0

import com.sunrain.bnetdisk.qmlplugin 1.0

import "../Actions"
import "../../Pages"

AppListener {
    id: downloadStore

    property alias downloadingModel: dlDelegate.tasks
    property alias completedModel: dlDelegate.completedTasks

    property string downloadPageviewComponentUri: Qt.resolvedUrl("../../UI/DownloadingTaskView.qml")

    BDiskDownloadDelegate {
        id: dlDelegate
    }

    function taskRunning(hash) {
        return dlDelegate.taskRunning(hash);
    }

    Filter {
        type: ActionTypes.downloadFile
        onDispatched: {
            //TODO unuse param atm
//            dlDelegate.download();
            var file = message.file;
            var saveName = message.saveName;
            var savePath = message.savePath;
            console.log("==== onDispatched downloadFile "+file+ " save to "+savePath+ " "+saveName);
            dlDelegate.download(file, savePath, saveName);
        }
    }

    Filter {
        type: ActionTypes.stopTask
        onDispatched: {
            var hash = message.hash;
            console.log("===== try to stop "+hash);
            dlDelegate.stop(hash);
        }
    }

    Filter {
        type: ActionTypes.startTask
        onDispatched: {
            var hash = message.hash;
            console.log("===== try to start "+hash);
            dlDelegate.resume(hash);
        }
    }

    Filter {
        type: ActionTypes.showDownloadingComponent
        onDispatched: {
            downloadPageviewComponentUri = Qt.resolvedUrl("../../UI/DownloadingTaskView.qml");
        }
    }

    Filter {
        type: ActionTypes.showCompletedComponent
        onDispatched: {
            downloadPageviewComponentUri = Qt.resolvedUrl("../../UI/CompletedDLTaskView.qml");
        }
    }
}
