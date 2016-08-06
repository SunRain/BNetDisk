import QtQuick 2.4
import Material 0.3
import Material.ListItems 0.1 as ListItem

import com.sunrain.bnetdisk.qmlplugin 1.0

import ".."
import "../QuickFlux/Stores"
import "../QuickFlux/Actions"
import "../QuickFlux/Scripts"
import "../Component"

Item {
    id: previewItem
    width: parent.width
    height: Const.screenHeight * 0.8

    QtObject {
        id: inner
        property int defaultIndex: OverlayViewStore.itemIndex
        onDefaultIndexChanged: {
            index = defaultIndex;
        }
        property int index: OverlayViewStore.itemIndex
        property var object: DirListStore.dirlistModel[inner.index]
        property var thumbObject: object[FileObjectKey.keyThumbs];
        property string thumbMiddle: thumbObject[FileObjectKey.keyThumbsMiddle]
        property string thumbLarge: thumbObject[FileObjectKey.keyThumbsLarge]
        property string icon: thumbObject[FileObjectKey.keyThumbsIcon]
        property string path: object[FileObjectKey.keyPath]
        property string fileName: AppUtility.fileObjectPathToFileName(path)
        property string date: AppUtility.formatDate(object[FileObjectKey.keyServerMTime])
        property string fromPath: AppUtility.fileObjectPathToPath(path)
    }

    Image {
        id: image
        width: parent.width - Const.largeSpace * 2
        anchors {
            top: parent.top
            topMargin: Const.middleSpace
            left: parent.left
            leftMargin: Const.largeSpace
            bottom: snapList.top
            bottomMargin: Const.middleSpace
        }
        fillMode: Image.PreserveAspectFit
        source: inner.thumbLarge

        MouseArea {
            id: hoverMouse
            property bool hover: false
            anchors.fill: parent
            hoverEnabled: true
            onExited: {
                hover = false;
            }
            onEntered: {
                hover = true;
            }
        }
        View {
            height: infoColumn.height
            width: parent.width
            anchors.bottom: parent.bottom
            Rectangle {
                anchors.fill: parent
                color: Qt.rgba(0,0,0,0.2)
            }
            Column {
                id: infoColumn
                width: parent.width - Const.tinySpace * 2
                x: Const.tinySpace
                Label {
                    width: parent.width
                    horizontalAlignment: Text.AlignLeft
                    wrapMode: Text.Wrap
                    text: inner.fileName
                }
                Label {
                    width: parent.width
                    horizontalAlignment: Text.AlignLeft
                    wrapMode: Text.Wrap
                    text: qsTr("From")+": "+inner.fromPath
                }
                Label {
                    width: parent.width
                    horizontalAlignment: Text.AlignLeft
                    wrapMode: Text.Wrap
                    text: qsTr("Date")+": "+inner.date
                }
            }
            opacity: hoverMouse.hover || leftBtn.hover || rightBtn.hover ? 1 : 0
            Behavior on opacity {
                NumberAnimation { duration: 500; easing.type: Easing.InOutQuad }
            }
        }
        BDIconButton {
            id: leftBtn
            iconName: "navigation/arrow_back"
            anchors {
                left: parent.left
                leftMargin: Const.middleSpace
                verticalCenter: parent.verticalCenter
            }
            onClicked: {
                if (inner.index > 0)
                    inner.index--;
            }
            opacity: hoverMouse.hover || hover || rightBtn.hover ? 1 : 0
            Behavior on opacity {
                NumberAnimation { duration: 500; easing.type: Easing.InOutQuad }
            }
        }
        BDIconButton {
            id: rightBtn
            iconName: "navigation/arrow_forward"
            anchors {
                right: parent.right
                rightMargin: Const.middleSpace
                verticalCenter: parent.verticalCenter
            }
            onClicked: {
                if (inner.index >= DirListStore.dirlistModel.length)
                    inner.index = DirListStore.dirlistModel.length;
                else
                    inner.index++;
            }
            opacity: hoverMouse.hover || hover || leftBtn.hover ? 1 : 0
            Behavior on opacity {
                NumberAnimation { duration: 500; easing.type: Easing.InOutQuad }
            }
        }
    }
    ListView {
        id: snapList
        orientation: ListView.Horizontal
        width: parent.width - Const.largeSpace * 2
        height: Const.itemHeight + Const.tinySpace * 2
        anchors {
            left: parent.left
            leftMargin: Const.largeSpace
            bottom: parent.bottom
            bottomMargin: Const.middleSpace
        }
        clip: true
        highlightFollowsCurrentItem: true
        highlight: Rectangle {
            color: Qt.rgba(0,0,0,0.2)
            width: ListView.view.cellWidth
            height: ListView.view.cellHeight
            x: ListView.view.currentItem.x
            y: ListView.view.currentItem.y
            Behavior on x { SpringAnimation { spring: 3; damping: 0.2 } }
            Behavior on y { SpringAnimation { spring: 3; damping: 0.2 } }
        }
        preferredHighlightBegin: (snapList.width - snapList.height) /2
        preferredHighlightEnd: (snapList.width + snapList.height) /2
        highlightRangeMode: ListView.StrictlyEnforceRange
        currentIndex: inner.index
//        onCurrentIndexChanged: {
//            console.log("===== onCurrentIndexChanged "+currentIndex);
//            snapList.positionViewAtIndex(currentIndex, ListView.Center)
//        }
//        Component.onCompleted: {
//            snapList.positionViewAtIndex(currentIndex, ListView.Center)
//        }
        model: DirListStore.dirlistModel
        delegate: MouseArea {
            width: height
            height: ListView.view.height
            onClicked: {
                inner.index = index;
            }
            property var object: DirListStore.dirlistModel[index]
            property var thumbObject: object[FileObjectKey.keyThumbs];
            property string icon: thumbObject[FileObjectKey.keyThumbsIcon]
            Image {
                width: Const.itemHeight
                height: width
                anchors.centerIn: parent
                fillMode: Image.PreserveAspectFit
                source: icon
            }
        }
    }

}
