import QtQuick 2.4
import Material 0.3
import Material.ListItems 0.1 as ListItem

import com.sunrain.bnetdisk.qmlplugin 1.0

import ".."
import "../QuickFlux/Stores"
import "../QuickFlux/Actions"
import "../QuickFlux/Scripts"
import "../UI"

import "../Script/Utility.js" as Utility

Page {
    id: searchPage

    property string searchKey

    actions: [
        Action{
            iconName: "action/search"
            name: qsTr("Search")
            onTriggered: {
                AppActions.search(searchKey);
            }
        }
    ]

    actionBar.customContent: Item {
        anchors.fill: parent
        TextField {
            anchors.verticalCenter: parent.verticalCenter
            text: ""
            onTextChanged: {
                searchKey = text;
            }
        }
    }

    ListView {
        anchors.fill: parent

        model: SearchStore.searchModel
    }
}
