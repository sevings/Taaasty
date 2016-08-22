import QtQuick 2.7
import QtQuick.Controls 2.0 as Q
import QtQuick.Controls.Material 2.0

Pane {
    id: dialog
    innerFlick: flick
    readonly property bool isLoginDialog: true // how to find this in the StackView?
    function clear() {
        mail.line = '';
        password.line = '';
    }
    Component.onCompleted: {
        mail.forceActiveFocus();
    }
    Poppable {
        body: dialog
    }
    Flickable {
        id: flick
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            right: parent.right
        }
        height: contentHeight > parent.height
                ? parent.height : contentHeight
        flickableDirection: Flickable.VerticalFlick
        topMargin: 2 * mm
        bottomMargin: 2 * mm
        contentWidth: parent.width
        contentHeight: column.height
        Poppable {
            body: dialog
        }
        Column {
            id: column
            width: parent.width
            spacing: 2 * mm
            Q.Label {
                id: mailLabel
                text: 'E-mail'
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                font.pointSize: window.fontBigger
            }
            LineInput {
                id: mail
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                width: 40 * mm
            }
            Q.Label {
                id: passwordLabel
                text: 'Пароль'
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                font.pointSize: window.fontBigger
            }
            LineInput {
                id: password // TODO: Enter key
                echoMode: TextInput.Password
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                width: 40 * mm
                onAccepted: {
                    if (loginButton.enabled)
                        Tasty.authorize(mail.text, password.text);
                }
            }
            ThemedButton {
                id: loginButton
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                text: 'Войти'
                width: 40 * mm
                onClicked: Tasty.authorize(mail.text, password.text)
                enabled: mail.text && password.text
            }
        //    ThemedButton {
        //        id: registerButton
        //        text: 'Зарегистрироваться'
        //        anchors.top: loginButton.bottom
        //        anchors.left: parent.left
        //        anchors.right: parent.right
        //        anchors.margins: 2 * mm
        //        enabled: mail.text && password.text
                //onClicked: Ctrl.signup()
        //    }
        }
    }
}
