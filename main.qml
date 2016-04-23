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
    StackView {
        id: stack
        anchors.fill: parent
        initialItem: feed
        delegate: StackViewDelegate {
            function transitionFinished(properties)
            {
                properties.exitItem.opacity = 1;
                properties.enterItem.opacity = 1;
            }
            pushTransition: StackViewTransition {
                PropertyAnimation {
                    target: enterItem
                    property: "opacity"
                    duration: 300
                    from: 0
                    to: 1
                }
                NumberAnimation {
                    target: enterItem
                    property: "x"
                    duration: 300
                    easing.type: Easing.InOutQuad
                    from: enterItem.width
                    to: 0
                }
                PropertyAnimation {
                    target: exitItem
                    property: "opacity"
                    duration: 300
                    from: 1
                    to: 0
                }
            }
            popTransition: StackViewTransition {
                PropertyAnimation {
                    target: exitItem
                    property: "opacity"
                    to: 0
                }
                NumberAnimation {
                    target: exitItem
                    property: "x"
                    duration: 300
                    easing.type: Easing.InOutQuad
                    to: exitItem.width
                }
                PropertyAnimation {
                    target: enterItem
                    property: "opacity"
                    duration: 300
                    from: 0
                    to: 1
                }
                PropertyAnimation {
                    target: enterItem
                    property: "scale"
                    duration: 300
                    from: 0.8
                    to: 1
                }
            }
        }

        Component {
            id: feed
            FeedView {
                onEntryClicked: {
                    stack.push({
                                   item: fullEntry,
                                   properties: {
                                       entry: currentEntry
                                   }
                               })
                }
                onPopped: stack.pop()
                poppable: Stack.index > 0
            }
        }
        Component {
            id: fullEntry
            FullEntryView {
                onPopped: stack.pop()
                poppable: Stack.index > 0
            }
        }
        Component {
            id: loginDialog
            LoginDialog { }
        }
        Connections {
            target: Tasty
            onAuthorizationNeeded: {
                stack.push(loginDialog);
            }
            onAuthorized: {
                stack.pop(loginDialog);
                loginDialog.clear();
            }
        }
//        MouseArea {
//            enabled: stack.depth > 1
//            anchors.fill: parent
//            drag.target: stack.currentItem
//            drag.axis: Drag.XAxis
//            drag.minimumX: 0
//            drag.maximumX: stack.width
//        }
    }
    MainMenu {
        onModeChanged: {
            stack.push({
                           item: feed,
                           properties: {
                               mode: mode
                           }
                       })
        }
    }
}
