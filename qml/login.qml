import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Controls 1.3

Window {
    id: login
    visible: true
    width: 200
    height: 200

//    MouseArea {
//        anchors.fill: parent
//        onClicked: {
//            Qt.quit();
//        }
//    }
    Connections {
        target: LoginProvider
        onLoginSuccess: {
            login.close();
        }
    }

    Column {
        width: parent.width
        Text {
            text: qsTr("Login")
//            anchors.centerIn: parent
        }

        Text {
            text: qsTr("User Name")
        }
        TextInput {
            width: parent.width
            height: 30
            onTextChanged: {
                LoginProvider.setUserName(text)
            }
        }
        Text {
            text: qsTr("Password")
        }
        TextInput {
            id: password
            width: parent.width
            height: 30
            onTextChanged: {
                LoginProvider.setPassWord(text)
            }
        }

        Text {
            text: qsTr("captch")
        }
        Image {
            width: parent.width
            fillMode: Image.PreserveAspectFit
            source: LoginProvider.captchaImgUrl
        }
        TextInput {
            id: captcah
            width: parent.width
            height: 30
        }

        Button {
            text: "login"
            onClicked: {
                LoginProvider.setCaptchaText(captcah.text);
                LoginProvider.login();
            }
        }
    }
}
