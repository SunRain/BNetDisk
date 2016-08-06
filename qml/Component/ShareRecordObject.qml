import QtQuick 2.0

QtObject {
    id: sr
    objectName: "ShareRecordObject"

    signal objectChanged

    property string shareId: ""
    property string shortlink: ""
    property string passwd: ""
    property string typicalPath: ""
    property string typicalCategory: ""
    property bool isPublicShare: false

    property var shareRecordObject:  ({
        "shareId": "",
        "fsIds": [],
        "channel": 0,
        "channelInfo": [],
        "status": 0,
        "expiredType": 0,
        "passwd": "0",
        "name": "0",
        "description": "",
        "ctime": "",
        "appId": "",
        "public": 1,
        "publicChannel": 0,
        "tplId": 0,
        "shorturl": "",
        "ip": "",
        "tag": 0,
        "shareinfo": "",
        "bitmap": 0,
        "port": 0,
        "dtime": "",
        "vCnt": 0,
        "dCnt": 0,
        "tCnt": 0,
        "shortlink": "",
        "typicalCategory": 6,
        "typicalPath": ""
    })
    onShareRecordObjectChanged: {
        shareId = shareRecordObject.shareId;
        shortlink = shareRecordObject.shortlink;
        passwd = shareRecordObject.passwd;
        typicalPath = shareRecordObject.typicalPath;
        typicalCategory = shareRecordObject.typicalCategory;
//        isPublicShare = shareRecordObject.public;

        objectChanged();
    }
}
