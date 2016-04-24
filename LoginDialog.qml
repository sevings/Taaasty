import QtQuick 2.3

Rectangle {
    id: dialog
//    anchors.fill: parent
    color: window.backgroundColor
    function clear() {
        mail.line = '';
        password.line = '';
    }
    Poppable {
        body: dialog
    }
    Text {
        id: mailLabel
        text: 'E-mail'
        anchors.margins: 20
        anchors.bottom: mail.top
        anchors.horizontalCenter: parent.horizontalCenter
        color: window.textColor
        font.pointSize: 25
    }
    LineInput {
        id: mail
        anchors.bottom: passwordLabel.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 20
        width: parent.width / 3 * 2
    }
    Text {
        id: passwordLabel
        text: 'Пароль'
        anchors.bottom: password.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 20
        color: window.textColor
        font.pointSize: 25
    }
    LineInput {
        id: password
//        echoMode: TextInput.Password
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 20
        width: parent.width / 3 * 2
        echoMode: TextInput.Password
    }
    ThemedButton {
        id: loginButton
        text: 'Войти'
        anchors.top: password.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 20
        onClicked: Tasty.authorize(mail.line, password.line)
    }
    ThemedButton {
        id: registerButton
        text: 'Зарегистрироваться'
        anchors.top: loginButton.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 20
        //onClicked: Ctrl.signup()
    }
}
