import QtQuick 2.7

Pane {
    id: dialog
//    anchors.fill: parent
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
    Text {
        id: mailLabel
        text: 'E-mail'
        anchors {
            margins: 2 * mm
            bottom: mail.top
            horizontalCenter: parent.horizontalCenter
        }
        color: window.textColor
        font.pointSize: window.fontBigger
    }
    LineInput {
        id: mail
        anchors {
            bottom: passwordLabel.top
            horizontalCenter: parent.horizontalCenter
            margins: 2 * mm
        }
        width: parent.width / 3 * 2
    }
    Text {
        id: passwordLabel
        text: 'Пароль'
        anchors {
            bottom: password.top
            horizontalCenter: parent.horizontalCenter
            margins: 2 * mm
        }
        color: window.textColor
        font.pointSize: window.fontBigger
    }
    LineInput {
        id: password // TODO: Enter key
        echoMode: TextInput.Password
        anchors {
            verticalCenter: parent.verticalCenter
            horizontalCenter: parent.horizontalCenter
            margins: 2 * mm
        }
        width: parent.width / 3 * 2
        onAccepted: {
            if (loginButton.enabled)
                Tasty.authorize(mail.text, password.text);
        }
    }
    ThemedButton {
        id: loginButton
        text: 'Войти'
        anchors {
            top: password.bottom
            left: parent.left
            right: parent.right
            margins: 2 * mm
        }
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
