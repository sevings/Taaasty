import QtQuick 2.3
import QtQuick.Controls 1.2

Rectangle {
    id: dialog
    anchors.fill: parent
    function clear() {
        mail.text = '';
        password.text = '';
    }
    Label {
        id: mailLabel
        text: 'E-mail'
        anchors.margins: 20
        anchors.bottom: mail.top
        anchors.horizontalCenter: parent.horizontalCenter
    }
    TextField {
        id: mail
        anchors.bottom: passwordLabel.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 20
    }
    Label {
        id: passwordLabel
        text: 'Пароль'
        anchors.bottom: password.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 20
    }
    TextField {
        id: password
        echoMode: TextInput.Password
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 20
    }
    ThemedButton {
        id: loginButton
        text: 'Войти'
        anchors.top: password.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 20
        onClicked: tasty.authorize(mail.text, password.text)
    }
    ThemedButton {
        id: registerButton
        text: 'Зарегистрироваться'
        anchors.top: loginButton.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 20
        //onClicked: Ctrl.signup()
    }
}
