pragma Singleton
import QtQuick 2.0
import QuickFlux 1.0

import com.sunrain.bnetdisk.qmlplugin 1.0

import "../Actions"

AppListener {
    id: overlayViewStore

    property string overlayViewUri: ""

    property int itemIndex: -1

    Filter {
        type: ActionTypes.askToOpenOverlayView
        onDispatched: {
            var view = message.view;
            if (view == "shareView") {
                overlayViewUri = Qt.resolvedUrl("../../UI/ShareLinkView.qml");
            } else if (view == "imagePreview") {
                overlayViewUri = Qt.resolvedUrl("../../UI/ImagePreviewView.qml");
                itemIndex = message.index;
            }
        }
    }
}
