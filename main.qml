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
    property color greenColor: '#00DF84'
    property color redColor: '#c00f19' // '#f3534b'
    property color darkGreen: Qt.darker(greenColor)
    property color darkRed: Qt.darker(redColor)
    property int fontBiggest: 30
    property int fontBigger: 25
    property int fontNormal: 20
    property int fontSmaller: 17
    property int fontSmallest: 14
    property bool unreadNotifications: NotifsModel.unread
//    property int unreadMessages: 0
    readonly property bool notifsShows: notifsView.state === "opened"
//    property bool showCommentMenu: false
//    property bool showSlugInput: false
//    property bool showConvers: false
//    property bool showDialog: false
    readonly property int anonymousId: 4409
    title: qsTr("Taaasty")
    color: backgroundColor
    onWidthChanged: {
        Tasty.setImageWidth(window.width - 2 * mm, window.width - 11 * mm);
    }
    Component.onCompleted: {
        Tasty.setImageWidth(window.width - 2 * mm, window.width - 11 * mm);
        loadingText.visible = false;
    }
    function showNotifs() {
        notifsView.state = "opened";
    }
    function hideNotifs() {
        notifsView.state = "closed";
        NotifsModel.markAsRead();
    }
    function toggleNotifs() {
        if (notifsView.y)
            showNotifs();
        else
            hideNotifs();
    }
    function showFooter() {
        footer.state = "opened";
    }
    function hideFooter() {
        footer.state = "closed";
    }
    function showLineInput(mode) {
        lineInput.state = "opened";
        lineInput.mode = mode;
        lineInput.forceActiveFocus();
    }
    function hideLineInput() {
        lineInput.state = "closed";
        lineInput.clear();
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
                console.log(entryId)
                stack.push({
                               item: fullEntry,
                               properties: {
                                   entryId: entryId
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
            backAnimation.start();
            setFooterFromStack();

            if (mode !== FeedModel.BetterThanMode)
                stack.currentItem.setMode(mode);
            else
                showLineInput('rating');
        }
        onTlogRequested: {
            backAnimation.start();
            showLineInput('tlog');
        }
        onSearchRequested: {
            backAnimation.start();
            showLineInput('query');
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
    InputDialog {
        id: lineInput
        onAccepted: {
            if (mode === 'tlog')
                stack.currentItem.setMode(FeedModel.TlogMode, undefined, lineInput.text);
            else if (mode === 'rating')
            {
                var r = Number(lineInput.text);
                if (r > 0)
                {
                    stack.currentItem.setMode(FeedModel.BetterThanMode);
                    stack.currentItem.minRating = r;
                }
            }
            else if (mode === 'query')
            {
                stack.currentItem.query = lineInput.text;
            }

            hideLineInput();
            setFooterFromStack();
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
