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
    property int fontBiggest: 30
    property int fontBigger: 25
    property int fontNormal: 20
    property int fontSmaller: 17
    property int fontSmallest: 14
    property bool cashedImages: false
    property bool unreadNotifications: notifsModel.unread
//    property int unreadMessages: 0
//    property bool showNotifs: false
//    property bool showCommentMenu: false
//    property bool showSlugInput: false
//    property bool showConvers: false
//    property bool showDialog: false
    title: qsTr("Taaasty")
    color: backgroundColor
    Component.onCompleted: {
        //tasty.getMe();
    }
    function showNotifs() {
        notifsView.y = 0;
    }
    function hideNotifs() {
        notifsView.y = window.height;
    }
    function toggleNotifs() {
        if (notifsView.y)
            showNotifs();
        else
            hideNotifs();
    }
    function showFooter() {
        footer.y = window.height - footer.height;
    }
    function hideFooter() {
        footer.y = window.height;
    }
    NotificationsModel {
        id: notifsModel
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
        onCurrentItemChanged: {
            if (!stack.currentItem)
                return;

            if (stack.currentItem.customTitle) {
                footer.title = stack.currentItem.title;
                footer.tlog = emptyTlog;
            }
            else {
                footer.title = '';
                footer.tlog = stack.currentItem.tlog;
            }

            window.hideFooter()
        }
        Tlog {
            id: emptyTlog
        }

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
                    target: enterItem
                    property: "scale"
                    duration: 300
                    from: 1.2
                    to: 1
                }
                PropertyAnimation {
                    target: exitItem
                    property: "opacity"
                    duration: 300
                    from: 1
                    to: 0
                }
                PropertyAnimation {
                    target: exitItem
                    property: "scale"
                    duration: 300
                    from: 1
                    to: 0.8
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
                    target: exitItem
                    property: "scale"
                    duration: 300
                    from: 1
                    to: 1.2
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
                                       entry: entry,
                                       showProfiles: mode !== FeedModel.AnonymousMode
                                   }
                               })
                }
                onAvatarClicked: {
                    stack.push({
                                   item: profile,
                                   properties: {
                                       tlog: tlog,
                                       author: author
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
                onAvatarClicked: {
                    stack.push({
                                   item: profile,
                                   properties: {
                                       tlogId: tlogId
                                   }
                               })
                }
            }
        }
        Component {
            id: profile
            ProfileView {
                onPopped: stack.pop()
                poppable: Stack.index > 0
                onTlogRequested: {
                    stack.push({
                                   item: feed,
                                   properties: {
                                       mode: FeedModel.TlogMode,
                                       tlogId: author.id
                                   }
                               })
                }
                onFollowersRequested: {
                    stack.push({
                                   item: users,
                                   properties: {
                                       mode: UsersModel.FollowersMode,
                                       tlogId: author.id,
                                       tlog: tlog
                                   }
                               })
                }
                onFollowingsRequested: {
                    stack.push({
                                   item: users,
                                   properties: {
                                       mode: UsersModel.FollowingsMode,
                                       tlogId: author.id,
                                       tlog: tlog
                                   }
                               })
                }
            }
        }
        Component {
            id: users
            UsersView {
                onPopped: stack.pop()
                poppable: Stack.index > 0
                onTlogRequested: {
                    stack.push({
                                   item: feed,
                                   properties: {
                                       mode: FeedModel.TlogMode,
                                       tlogId: tlog
                                   }
                               })
                }
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
//                loginDialog.clear();
            }
        }
    }
    MainMenu {
        onModeChanged: {
            var item = stack.find(function (item) {
                return item.mode === mode //&& (mode !== FeedModel.TlogMode || item.tlog === tlog)
            });
            if (item) {
//                console.log('found');
                stack.pop(item);
                return;
            }

            stack.push({
                           item: feed,
                           properties: {
                               mode: mode,
                               tlogId: 281926
                           }
                       })
        }
    }
    NotificationsView {
        id: notifsView
        anchors.left: parent.left
        anchors.right: parent.right
        height: parent.height
        y: height
        notifs: notifsModel
        z: 5
        Behavior on y {
            NumberAnimation {
                duration: 300
            }
        }
        onAvatarClicked: {
            stack.push({
                           item: profile,
                           properties: {
                               tlogId: tlog
                           }
                       })
        }
        onEntryRequested: {
            stack.push({
                           item: fullEntry,
                           properties: {
                               entryId: entry,
                               showProfiles: showProfile
                           }
                       })
        }
    }
    Footer {
        id: footer
        onAvatarClicked: {
            stack.push({
                           item: profile,
                           properties: {
                               tlog: tlog
                           }
                       })
        }
    }
}
