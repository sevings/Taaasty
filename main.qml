import QtQuick 2.6
import QtQuick.Controls 1.5

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Taaasty")
    Component.onCompleted: {

    }
    LoginDialog {
        id: loginDialog
        visible: false
        Connections {
            target: tasty
            onAuthorizationNeeded: {
                loginDialog.visible = true;
            }
            onAuthorized: {
                loginDialog.clear();
                loginDialog.visible = false;
            }
        }
    }

}
