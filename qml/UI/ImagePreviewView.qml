import QtQuick 2.4
import Material 0.3
import Material.ListItems 0.1 as ListItem

import com.sunrain.bnetdisk.qmlplugin 1.0

import ".."
import "../QuickFlux/Stores"
import "../QuickFlux/Actions"
import "../QuickFlux/Scripts"

Item {
    id: previewItem
    width: parent.width
    height: left.height

    Item {
        id: left
        width: parent.width * 0.8
        height: column.height + Const.tinySpace * 2
        Column {
            id: column
            width: parent.width
            anchors.verticalCenter: parent.verticalCenter
            spacing: Const.tinySpace
            Image {
                id: image
                width: parent.width - Const.largeSpace * 2
                height: width
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                fillMode: Image.PreserveAspectFit
            }
            ListView {
                id: snapList
                orientation: ListView.Horizontal
                width: parent.width
                height: Const.itemHeight
                clip: true
                highlightFollowsCurrentItem: true
                highlight: Rectangle {
                    color: "#ee840a"
                    width: view.cellWidth
                    height: view.cellHeight
                    radius: 5
                    x: view.currentItem.x
                    y: view.currentItem.y
                    Behavior on x { SpringAnimation { spring: 3; damping: 0.2 } }
                    Behavior on y { SpringAnimation { spring: 3; damping: 0.2 } }

                }
                currentIndex: OverlayViewStore.itemIndex
                onCurrentIndexChanged: {
                    snapList.positionViewAtIndex(currentIndex, ListView.Center)
                }

                model: DirListStore.dirlistModel
                delegate: MouseArea {
                    width: height
                    height: ListView.view.height
                    onClicked: {
                        snapList.currentIndex = index;
                    }

                    property var object: DirListStore.dirlistModel[index]
                    property var thumbObject: object[FileObjectKey.keyThumbs];
                    property string thumbMiddle: thumbObject[FileObjectKey.keyThumbsMiddle]
                    property string thumbLarge: thumbObject[FileObjectKey.keyThumbsLarge]
                    property string icon: thumbObject[FileObjectKey.keyThumbsIcon]
                    Image {
                        anchors.fill: parent
                        fillMode: Image.PreserveAspectFit
                        source: icon
                    }
                }
            }
        }
    }


}
