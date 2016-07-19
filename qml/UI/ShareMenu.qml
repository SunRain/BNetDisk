import QtQuick 2.0
import QuickFlux 1.0
import Material 0.3
import Material.ListItems 0.1 as ListItem

import com.sunrain.bnetdisk.qmlplugin 1.0

import "../QuickFlux/Stores"

Dropdown {
    id: shareMenu

    height: options.height

    property string shareId: ""

    Rectangle {
        anchors.fill: parent
        radius: 2 * Units.dp
    }
    Column {
        id: options
        width: parent.width
        ListItem.Standard {
            text: qsTr("share public")
            onClicked: {
                ShareStore.pubShare(shareId);
            }
        }
        ListItem.Standard {
            text: qsTr("share private")
            onClicked: {
                ShareStore.privShare(shareId);
            }
        }
    }
}
