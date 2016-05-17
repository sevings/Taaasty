import QtQuick 2.5

Rectangle {
    id: dialog
//    anchors.fill: parent
    readonly property bool isLoginDialog: true // how to find this in the StackView?
    color: window.backgroundColor
    signal popped
    property bool poppable
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
        anchors.margins: 2 * mm
        anchors.bottom: mail.top
        anchors.horizontalCenter: parent.horizontalCenter
        color: window.textColor
        font.pointSize: window.fontBigger
    }
    LineInput {
        id: mail
        anchors.bottom: passwordLabel.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 2 * mm
        width: parent.width / 3 * 2
    }
    Text {
        id: passwordLabel
        text: 'Пароль'
        anchors.bottom: password.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 2 * mm
        color: window.textColor
        font.pointSize: window.fontBigger
    }
    LineInput {
        id: password // TODO: Enter key
        echoMode: TextInput.Password
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 2 * mm
        width: parent.width / 3 * 2
    }
    ThemedButton {
        id: loginButton
        text: 'Войти'
        anchors.top: password.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 2 * mm
        onClicked: Tasty.authorize(mail.line, password.line)
        enabled: mail.line && password.line
    }
    ThemedButton {
        id: registerButton
        text: 'Зарегистрироваться'
        anchors.top: loginButton.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 2 * mm
        enabled: mail.line && password.line
        //onClicked: Ctrl.signup()
    }
}
