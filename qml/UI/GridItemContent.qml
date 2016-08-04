import QtQuick 2.4
import Material 0.3
import Material.ListItems 0.1 as ListItem

import com.sunrain.bnetdisk.qmlplugin 1.0

import ".."
import "../QuickFlux/Stores"
import "../QuickFlux/Actions"
import "../QuickFlux/Scripts"

import "../Script/Utility.js" as Utility

Item {
    id: gridItemContent
    width: parent.width
    height: parent.height

    property int gridColumns: width > Const.cardSize ? width/Const.cardSize : 1

    signal menuClicked(var parentItem, var path)
    signal imageClicked(var thumbLarge)

    GridView {
        id: gridView

        width: Const.cardSize * gridColumns
        height: parent.height

        x: (parent.width - gridView.width)/2

        cellWidth: Const.cardSize
        cellHeight: Const.cardSize + 48 * Units.dp + Const.tinySpace * 2 //card.height
        clip: true

        add: Transition { NumberAnimation { properties: "x" } }
        remove: Transition { NumberAnimation { properties: "x" } }

//        highlight: Rectangle {
//            color: "#ee840a"
//            width: view.cellWidth
//            height: view.cellHeight
//            radius: 5
//            x: view.currentItem.x
//            y: view.currentItem.y
//            Behavior on x { SpringAnimation { spring: 3; damping: 0.2 } }
//            Behavior on y { SpringAnimation { spring: 3; damping: 0.2 } }

//        }

        model: DirListStore.dirlistModel
        delegate: Item {
            id: cardItem
            width: Const.cardSize
            height: card.height + Const.tinySpace * 2
            Card {
                id: card
                width: Const.cardSize - Const.tinySpace * 2
                height: Const.cardSize + 48 * Units.dp // 48 * Units.dp for ListItem.Standard
                x: Const.tinySpace
                y: Const.tinySpace

                property var object: DirListStore.dirlistModel[index] //.get(index)
                property bool isDir: object[FileObjectKey.keyIsdir] == 1
                property string path: object[FileObjectKey.keyPath]
                property string fileName: AppUtility.fileObjectPathToFileName(path)
                property string mtime: object[FileObjectKey.keyServerMTime]
                property string fsID: object[FileObjectKey.keyFsId]
                property int category: object[FileObjectKey.keyCategory]
                property int size: object[FileObjectKey.keySize]
                property var thumbObject: object[FileObjectKey.keyThumbs];
                property string thumbMiddle: thumbObject[FileObjectKey.keyThumbsMiddle]
                property string thumbLarge: thumbObject[FileObjectKey.keyThumbsLarge]

                Rectangle {
                    anchors.fill: parent
                }

                Column {
                    id: column
                    width: parent.width
                    Image {
                        id: image
                        width: parent.width
                        height: width
                        fillMode: Image.PreserveAspectCrop//Image.PreserveAspectFit
                        horizontalAlignment: Image.AlignHCenter
                        verticalAlignment: Image.AlignVCenter
                        source: card.thumbMiddle
                        Ink {
                            id: ink
                            anchors.fill: parent
                            enabled: true
                            onClicked: {
                                imageClicked(card.thumbLarge)
                            }
                        }
                    }
                    ListItem.Standard {
                        id: infoBanner
                        text: card.fileName
                        secondaryItem: IconButton {
                            anchors.centerIn: parent
                            action: Action {
                                iconName: "navigation/more_vert"
                            }
                            onClicked: {
                                menuClicked(infoBanner, card.path)
                            }
                        }
                    }
                }
            }
        }
    }

    Scrollbar {
        flickableItem: gridView
    }

}
