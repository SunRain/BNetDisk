import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Controls 1.2

import Material 0.3
import Material.ListItems 0.1 as ListItem

ApplicationWindow {
    id: login
    visible: true
    width: 600 * Units.dp
    height: 400 * Units.dp

    property string st: "ShowLoginView"
    property bool captchVisible: captchaImg != undefined && captchaImg != ""
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
    }

    View {
        anchors.fill: parent
        elevation: 1
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
                to: "*"
                NumberAnimation { duration: 500; easing.type: Easing.Linear}
            }
        ]
        Loader {
            id: loader
            anchors.fill: parent
        }

        ProgressView {
            id: progressView
        }

    }

    Component {
        id: loginComponent
        Item {
            id: loginView
            anchors.fill: parent
            enabled: opacity == 1
            Column {
                id: loginColumn
                width: parent.width - dp(32)
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
                        onTextChanged: {
                            login.password = text
                        }
                    }
                }
                ListItem.Standard {
                    id: captha
                    visible: captchVisible
                    enabled: captchVisible
                    opacity: captchVisible ? 1 : 0
                    Behavior on opacity {
                        NumberAnimation { duration: 400 }
                    }
                    action: Icon {
                        anchors.centerIn: parent
                        name: "action/info"
                    }
                    content: Row {
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: dp(8)
                        TextField {
                            width: captha.width/2
                            anchors.verticalCenter: parent.verticalCenter
                            text: qsTr("captcha")
                            onTextChanged: {
                                LoginProvider.captchaText = text;
                            }
                        }
                        Image {
                            height: captha.height * 0.8
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
                Button {
                    text: qsTr("SIGN IN")
                    textColor: Theme.accentColor
                    anchors.horizontalCenter: parent.horizontalCenter
                    elevation: 1
                    onClicked: {
                        LoginProvider.login();
                        login.st = "ShowProgressView";
                    }
                }
            }
        }
    }
    Component {
        id: errorComponent
        Item {
            id: errorView
            anchors.fill: parent
            enabled: opacity == 1
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
            Button {
                text: qsTr("BACK")
                textColor: Theme.accentColor
                anchors{
                    bottom: parent.bottom
                    bottomMargin: dp(48)
                    horizontalCenter: parent.horizontalCenter
                }
                elevation: 1
                onClicked: {
                    login.st = "ShowLoginView";
                }
            }
        }
    }
}
