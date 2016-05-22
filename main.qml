import QtQuick 2.5
import QtQuick.Controls 1.4
import org.binque.taaasty 1.0

ApplicationWindow {
    id: window
    visible: true
    width: 480
    height: 800
    property color backgroundColor: 'black'
    property color textColor: 'white'
    property color secondaryTextColor: 'darkgrey'
    property color greenColor: '#00DF84'
    property color redColor: '#c00f19' // '#f3534b'
    property int fontBiggest: 30
    property int fontBigger: 25
    property int fontNormal: 20
    property int fontSmaller: 17
    property int fontSmallest: 14
    property bool unreadNotifications: NotifsModel.unread
//    property int unreadMessages: 0
//    property bool showNotifs: false
//    property bool showCommentMenu: false
//    property bool showSlugInput: false
//    property bool showConvers: false
//    property bool showDialog: false
    readonly property int anonymousId: 4409
    title: qsTr("Taaasty")
    color: backgroundColor
    Component.onCompleted: {
        loadingText.visible = false;
    }
    function showNotifs() {
        notifsView.y = 0;
    }
    function hideNotifs() {
        notifsView.y = window.height;
        NotifsModel.markAsRead();
    }
    function toggleNotifs() {
        if (notifsView.y)
            showNotifs();
        else
            hideNotifs();
    }
    function showFooter() {
        footer.y =  -footer.height;
    }
    function hideFooter() {
        footer.y = 0;
    }
    function setFooterTlog(tlog) {
        if (!tlog)
            tlog = emptyTlog;

        footer.tlog = tlog;
    }
    function setFooterFromStack() {
        if (stack.currentItem.customTitle) {
            footer.title = stack.currentItem.title;
            setFooterTlog();
        }
        else {
            footer.title = '';
            setFooterTlog(stack.currentItem.tlog);
        }
    }
    function openLink(url) {
        var matches = /taaasty.com\/(?:~|%7E)([^\/]+)(?:\/([\d]+))?/.exec(url);
        if (matches) {
            var slug = matches[1];
            var entryId = matches[2];
            if (entryId) {
                stack.push({
                               item: fullEntry,
                               properties: {
                                   entry: entryId
                               }
                           })
            }
            else if (slug) {
                stack.push({
                               item: profile,
                               properties: {
                                   slug: slug
                               }
                           })
            }
            else
                Qt.openUrlExternally(url)
        }
        else
            Qt.openUrlExternally(url)
    }
    Tlog {
        id: emptyTlog
    }
    BusyBar {
        id: bar
        z: 100
        busy: Tasty.busy > 0
    }
    Splash {
        id: loadingText
    }
    MainMenu {
        id: menu
        visible: stack.depth === 1 && stack.currentItem && stack.currentItem.x > 0
        onModeChanged: {
            stack.currentItem.mode = mode;
            if (mode === FeedModel.TlogMode)
                stack.currentItem.tlogId = 286801;

            backAnimation.start();
            setFooterFromStack();
        }
        onVisibleChanged: {
            window.hideFooter();
        }
        onWaterTlogsRequested: {
            backAnimation.start();
            stack.push({
                           item: users,
                           properties: {
                               mode: UsersModel.WaterMode
                           }
                       })
        }
        onFireTlogsRequested: {
            backAnimation.start();
            stack.push({
                           item: users,
                           properties: {
                               mode: UsersModel.FireMode
                           }
                       })
        }
        onLoginRequested: {
            backAnimation.start();
            stack.push(loginDialog);
        }
    }
    ParallelAnimation {
        id: backAnimation
        PropertyAnimation {
            target: stack.currentItem
            property: "x"
            to: 0
            duration: 300
        }
        PropertyAnimation {
            target: stack.currentItem
            property: "opacity"
            to: 1
            duration: 300
        }
    }
    StackView {
        id: stack
        anchors.fill: parent
        initialItem: feed
        onCurrentItemChanged: {
            if (!stack.currentItem)
                return;

            setFooterFromStack();

            window.hideFooter();

            if (stack.currentItem.isFeedView)
                stack.currentItem.pushed();
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
                onFlowClicked: {
                    stack.push({
                                   item: feed,
                                   properties: {
                                       mode: FeedModel.TlogMode,
                                       tlogId: flowId
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
                onProfileRequested: {
                    stack.push({
                                   item: profile,
                                   properties: {
                                       tlogId: tlog
                                   }
                               })
                }
            }
        }
        Component {
            id: loginDialog
            LoginDialog {
                onPopped: stack.pop()
                poppable: Stack.index > 0
            }
        }
        Connections {
            target: Tasty
            onAuthorizationNeeded: {
                if (!stack.find(function (item) {
                    return item.isLoginDialog;
                }))
                    stack.push(loginDialog);
            }
            onAuthorized: {
                stack.pop(loginDialog);
            }
        }
    }
    NotificationsView {
        id: notifsView
        anchors.left: parent.left
        anchors.right: parent.right
        height: parent.height
        y: height
        z: 5
        visible: y < height
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
            if (stack.currentItem.isFullEntryView && stack.currentItem.entryId === entry)
                return;

            stack.push({
                           item: fullEntry,
                           properties: {
                               entryId: entry,
                               showProfiles: showProfile
                           }
                       })
        }
        onTlogRequested: {
            if (stack.currentItem.isFeedView
                    && stack.currentItem.mode === FeedModel.TlogMode
                    && stack.currentItem.tlogId === tlogId)
                return;

            stack.push({
                           item: feed,
                           properties: {
                               mode: FeedModel.TlogMode,
                               tlogId: tlogId
                           }
                       })
        }
    }
    Rectangle {
        id: fakeFooter
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 0
        z: 20
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
    Dialog {
        id: dialog
        z: 100
        Connections {
            target: Tasty
            onError: {
                dialog.show((code ? code + '\n' : '') + text);
            }
        }
    }
}
