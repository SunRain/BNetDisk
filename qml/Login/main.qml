import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Controls 1.2

import Material 0.3
import Material.ListItems 0.1 as ListItem

ApplicationWindow {
    id: login
    visible: true
    width: 800 * Units.dp
    height: 600 * Units.dp

    property string st: "ShowProgressView" //"ShowLoginView"
    property bool captchaVisible: captchaImg != undefined && captchaImg != ""
    property string captchaImg
    property string errorText

    property string userName
    property string password
    onUserNameChanged: {
        LoginProvider.userName = userName;
    }
    onPasswordChanged: {
        LoginProvider.passWord = password;
    }

    Component.onCompleted: {
        console.log("====== onCompleted st "+st);
        if (st == "ShowProgressView")
            LoginProvider.loginByCookie();
    }

    iconHelper {
        useQtResource: true
        /**************************
        if set useQtResource to true, alternativePath should be the prefix value in qrc
        if set useQtResource to false
            if using material system-wide icons, alternativePath should be setted to empty, Eg. ""
            if using application alternative icons, alternativePath should be setted to full path of icon path
        ***************************/
        alternativePath: "/"// iconHelper.applicationPath +"/image/icons"
    }

    theme {
        primaryColor: "blue"
        accentColor: "red"
        tabHighlightColor: "white"
    }

    Image {
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
        source: Qt.resolvedUrl("../../image/image/login_bg.png")
    }

    Connections {
        target: LoginProvider
        onLoginSuccess: {
            login.close();
        }
        onCaptchaImgUrlChanged: {
            login.captchaImg = captchaImgUrl;
            login.st = "ShowLoginView";
        }
        onLoginAbort: {
            console.log("====== onLoginAbort");
            login.errorText = qsTr("Login aborted!!");
            login.st = "ShowErrorView";
        }
        onLoginFailure: { //message
            console.log("====== onLoginFailure " + message);
            login.errorText = message;
            login.st = "ShowErrorView";
        }
        onLoginByCookieSuccess: {
            console.log("====== onLoginByCookieSuccess ");
            login.close();
        }
        onLoginByCookieFailure: { //message
            console.log("====== onLoginByCookieFailure " + message);
            login.errorText = message;
            login.st = "ShowErrorView";
        }
    }

    View {
        width: parent.width * 0.8
        height: parent.height * 0.8
        anchors.centerIn: parent

        elevation: 4
        radius: dp(2)

        state: login.st
        states: [
            State {
                name: "ShowLoginView"
                PropertyChanges { target: progressView; opacity: 0}
                PropertyChanges { target: loader; sourceComponent: loginComponent}
            },
            State {
                name: "ShowProgressView"
                PropertyChanges { target: progressView; opacity: 1}
                PropertyChanges { target: loader; sourceComponent: loader.Null}
            },
            State {
                name: "ShowErrorView"
                PropertyChanges { target: progressView; opacity: 0}
                PropertyChanges { target: loader; sourceComponent: errorComponent}
            }
        ]
        transitions: [
            Transition {
                from: "ShowProgressView"
                to: "ShowErrorView"
                RotationAnimation {
                    target: actionButton
                    property: "rotation"
                    from: 0
                    to: 180
                    duration: 500
                }
            },
            Transition {
                from: "ShowErrorView"
                to: "ShowLoginView"
                RotationAnimation {
                    target: actionButton
                    property: "rotation"
                    from: 180
                    to: 0
                    duration: 500
                }
            }
        ]

        Image {
            width: parent.width
            height: parent.height / 2
            anchors.top: parent.top
            fillMode: Image.PreserveAspectCrop
            source: Qt.resolvedUrl("../../image/image/login_bg.png")
        }
        Loader {
            id: loader
            width: parent.width * 0.6
            height: parent.height * 0.5
            anchors.centerIn: parent
        }

        ProgressView {
            id: progressView
        }

        ActionButton {
            id: actionButton
            anchors {
                verticalCenter: loader.verticalCenter
                horizontalCenter: loader.right
            }
            action: Action {
                iconName: "navigation/arrow_forward" //login.st == "ShowLoginView" ? "navigation/arrow_forward" : "navigation/arrow_back"
                text: login.st == "ShowLoginView" ? qsTr("Login") : qsTr("Back")
            }
            onClicked: {
                if (login.st == "ShowLoginView") {
                    LoginProvider.login();
                    login.st = "ShowProgressView";
                } else { //show error view
                    login.st = "ShowLoginView";
                }
            }
        }

    }

    Component {
        id: loginComponent
        View {
            id: loginView
            anchors.fill: parent
            elevation: 4
            radius: dp(2)

            Column {
                id: loginColumn
                width: parent.width - dp(64)
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter

                Label {
                    width: parent.width
                    style: "title"
                    horizontalAlignment: Text.AlignHCenter
                    text: qsTr("Login")
                }
                Item {
                    width: parent.width
                    height: dp(16)
                }
                ListItem.Standard {
                    id: titleItem
                    action: Icon {
                        anchors.centerIn: parent
                        name: "action/account_box"
                    }
                    content: TextField {
                        anchors.centerIn: parent
                        width: parent.width
                        placeholderText: qsTr("user name")
                        text: login.userName
                        focus: true
                        onTextChanged: {
                            login.userName = text
                        }
                    }
                }
                ListItem.Standard {
                    id: passwordItem
                    action: Icon {
                        anchors.centerIn: parent
                        name: "action/lock"
                    }
                    content: TextField {
                        anchors.centerIn: parent
                        width: parent.width
                        echoMode: TextInput.Password
                        placeholderText: qsTr("password")
                        text: login.password
                        focus: true
                        onTextChanged: {
                            login.password = text
                        }
                    }
                }
                ListItem.Standard {
                    id: captcha
                    visible: captchaVisible
                    enabled: captchaVisible
                    opacity: captchaVisible ? 1 : 0
                    Behavior on opacity {
                        NumberAnimation { duration: 400 }
                    }
                    action: Icon {
                        anchors.centerIn: parent
                        name: "action/info"
                    }
                    content: Item {
                        anchors.fill: parent
                        TextField {
                            anchors {
                                left: parent.left
                                right: captchaRow.left
                                rightMargin: dp(8)
                                verticalCenter: parent.verticalCenter
                            }
                            text: qsTr("captcha")
                            focus: true
                            onTextChanged: {
                                LoginProvider.captchaText = text;
                            }
                        }
                        Row {
                            id: captchaRow
                            spacing: dp(8)
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.right: parent.right
                            Image {
                                height: captcha.height * 0.8
                                fillMode: Image.PreserveAspectFit
                                source: login.captchaImg
                            }
                            IconButton {
                                anchors.verticalCenter: parent.verticalCenter
                                iconName: "navigation/refresh"
                                onClicked: {
                                    LoginProvider.login();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    Component {
        id: errorComponent
        View {
            id: errorView
            anchors.fill: parent
            elevation: 4
            radius: dp(2)
            Column {
                width: parent.width - dp(32)
                anchors{
                    top: parent.top
                    topMargin: dp(48)
                    horizontalCenter: parent.horizontalCenter
                }
                spacing: dp(32)
                Label {
                    width: parent.width
                    style: "title"
                    text: qsTr("LOGIN FAILURE")
                    horizontalAlignment: Text.AlignHCenter
                }
                Label {
                    width: parent.width
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    text: login.errorText
                }
            }
        }
    }
}
