pragma Singleton
import QtQuick 2.0
import QuickFlux 1.0
import QSyncable 1.0

import com.sunrain.bnetdisk.qmlplugin 1.0

import "../Actions"

AppListener {
    id: shareStore

    property string sharelink: ""
    property string password: ""
    property bool showErrorLabel: false
    property var shareRecordList

    property bool refreshing: false
}
