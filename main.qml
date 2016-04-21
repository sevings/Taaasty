import QtQuick 2.3
import QtQuick.Controls 1.2
import org.binque.taaasty 1.0

ApplicationWindow {
    id: window
    visible: true
    width: 480
    height: 800
    property color backgroundColor: 'black'
    property color textColor: 'white'
    property color secondaryTextColor: 'darkgrey'
    property color brightColor: '#00DF84'
    property bool unreadNotifications: false
    property int unreadMessages: 0
    property bool showNotifs: false
    property bool showCommentMenu: false
    property bool showSlugInput: false
    property bool showConvers: false
    property bool showDialog: false
    property string mode: 'my'
    property string secondMode: 'none'
    title: qsTr("Taaasty")
    color: backgroundColor
    Component.onCompleted: {
        //tasty.getMe();
    }
    BusyBar {
        id: bar
        z: 100
        busy: Tasty.busy > 0
    }
    LoginDialog {
        id: loginDialog
        visible: false
        Connections {
            target: Tasty
            onAuthorizationNeeded: {
                loginDialog.visible = true;
            }
            onAuthorized: {
                loginDialog.clear();
                loginDialog.visible = false;
            }
        }
    }
    FeedView {
        id: live
        model: FeedModel {
            mode: FeedModel.ExcellentMode
        }
    }
//    FullEntryView {
//        id: fullEntry
//        entryId: 21046324
//    }
}
