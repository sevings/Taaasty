import QtQuick 2.7
import QtQuick.Controls 2.0
import org.binque.taaasty 1.0
import ImageCache 2.0

ApplicationWindow {
    id: window
    visible: true
    width: 540
    height: 960
    readonly property bool darkTheme: Settings.darkTheme
    property color backgroundColor: darkTheme ? 'black' : '#eff3f7'
    property color textColor: darkTheme ? 'white' : '#424d52'
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
    property CachedImage savingImage
    title: qsTr("Taaasty")
    color: backgroundColor
    onWidthChanged: {
        Tasty.setImageWidth(window.width - 2 * mm, window.width - 11 * mm);
    }
    Component.onCompleted: {
        Tasty.setImageWidth(window.width - 2 * mm, window.width - 11 * mm);
        loadingText.visible = false;
    }
    onClosing: {
        if (notifsShows) {
            hideNotifs();
            close.accepted = false;
        }
        else if (stack.depth > 1) {
            stack.pop();
            close.accepted = false;
        }
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
        inputDialog.state = "opened";
        inputDialog.mode = mode;
    }
    function hideLineInput() {
        inputDialog.state = "closed";
        inputDialog.clear();
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
        var matches = /taaasty.com\/(?:~|%7E)?([^\/]+)(?:\/([\d]+))?/.exec(url);
        if (matches) {
            var slug = matches[1];
            var entryId = matches[2];
            if (entryId) {
                pushFullEntryById(entryId);
            }
            else if (slug && slug !== 'anonymous') {
                pushProfileBySlug(slug);
            }
            else
                Qt.openUrlExternally(url)
        }
        else
            Qt.openUrlExternally(url)
    }
    function saveImage(image) {
        savingImage = image;
        inputDialog.text = image.fileName;
        showLineInput('save');
    }
    function pushChats() {
        stack.push(stack.chats,
                   {
                       poppable: true
                   }
                   )
    }
    function pushMessages(chat) {
        stack.push(stack.messages,
                   {
                       chat: chat,
                       poppable: true
                   }
                   )
    }
    function pushFullEntry(entry) {
        stack.push(stack.fullEntry,
                   {
                       entry: entry,
                       poppable: true
                   }
                   )
    }
    function pushFullEntryById(entryId) {
        if (stack.currentItem.isFullEntryView && stack.currentItem.entryId === entryId)
            return;

        stack.push(stack.fullEntry,
                   {
                       entryId: entryId,
                       poppable: true
                   }
                   )
    }
    function pushProfile(tlog, author) {
        stack.push(stack.profile,
                   {
                       tlog: tlog,
                       author: author,
                       poppable: true
                   }
                   )
    }
    function pushProfileById(tlogId) {
        stack.push(stack.profile,
                   {
                       tlogId: tlogId,
                       poppable: true
                   }
                   )
    }
    function pushProfileBySlug(slug) {
        stack.push(stack.profile,
                   {
                       slug: slug,
                       poppable: true
                   }
                   )
    }
    function pushTlog(tlogId) {
        if (stack.currentItem.isFeedView
                && stack.currentItem.mode === FeedModel.TlogMode
                && stack.currentItem.tlogId === tlogId)
            return;

        stack.push(feed,
                   {
                       mode: FeedModel.TlogMode,
                       tlogId: tlogId,
                       poppable: true
                   }
                   )
    }
    function pushCalendar(tlogId) {
        stack.push(stack.calendar,
                   {
                       tlogId: tlogId,
                       poppable: true
                   }
                   )
    }
    function pushUsers(mode, authorId, tlog) {
        stack.push(stack.users,
                   {
                       mode: mode,
                       tlogId: authorId,
                       tlog: tlog,
                       poppable: true
                   }
                   )
    }
    function pushLoginDialog() {
        stack.push(stack.loginDialog,
                   {
                       poppable: true
                   }
                   )
    }
    function pushTrainingProgress(full) {
        stack.push(stack.trainingProgress,
                   {
                       fullLoad: full,
                       poppable: false
                   }
                   )
    }
    function pushAbout() {
        stack.push(stack.about,
                   {
                       poppable: true
                   })
    }
    function popFromStack() {
        stack.pop();
    }
    function handleInputLine(mode, text) {
        if (!text)
            return;

        if (mode === 'tlog')
            stack.currentItem.setMode(FeedModel.TlogMode, undefined, text);
        else if (mode === 'rating') {
            var r = Number(text);
            if (r > 0)
            {
                stack.currentItem.setMode(FeedModel.BetterThanMode);
                stack.currentItem.minRating = r;
            }
        }
        else if (mode === 'query') {
            stack.currentItem.query = text;
        }
        else if (mode === 'save') {
            savingImage.saveToFile(text);
            savingImage = Cache.image();
        }
        hideLineInput();
        setFooterFromStack();
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

            if (mode !== FeedModel.BetterThanMode)
                stack.currentItem.setMode(mode);
            else
                showLineInput('rating');

            setFooterFromStack();
        }
        onTlogRequested: {
            backAnimation.start();
            showLineInput('tlog');
        }
        onChatsRequested: {
            backAnimation.start();
            window.pushChats();
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
            window.pushUsers(UsersModel.WaterMode);
        }
        onFireTlogsRequested: {
            backAnimation.start();
            window.pushUsers(UsersModel.FireMode);
        }
        onLoginRequested: {
            backAnimation.start();
            window.pushLoginDialog();
        }
        onAboutRequested: {
            backAnimation.start();
            window.pushAbout();
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
        popEnter: Transition {
            PropertyAnimation {
                property: "opacity"
                duration: 300
                from: 0
                to: 1
            }
            PropertyAnimation {
                property: "scale"
                duration: 300
                from: 0.8
                to: 1
            }
        }
        popExit: Transition {
            PropertyAnimation {
                property: "opacity"
                duration: 300
                to: 0
            }
            NumberAnimation {
                property: "x"
                duration: 300
                easing.type: Easing.InOutQuad
                to: window.width
            }
            PropertyAnimation {
                property: "scale"
                duration: 300
                from: 1
                to: 1.2
            }
        }
        pushEnter: Transition {
            PropertyAnimation {
                property: "opacity"
                duration: 300
                from: 0
                to: 1
            }
            NumberAnimation {
                property: "x"
                duration: 300
                easing.type: Easing.InOutQuad
                from: window.width
                to: 0
            }
            PropertyAnimation {
                property: "scale"
                duration: 300
                from: 1.2
                to: 1
            }
        }
        pushExit: Transition {
            PropertyAnimation {
                property: "opacity"
                duration: 300
                from: 1
                to: 0
            }
            PropertyAnimation {
                property: "scale"
                duration: 300
                from: 1
                to: 0.8
            }
        }
        Component {
            id: feed
            FeedView {
                poppable: StackView.index > 0
            }
        }
        property Component chats:               Qt.createComponent("ChatsView.qml",         Component.Asynchronous, stack);
        property Component messages:            Qt.createComponent("MessagesView.qml",      Component.Asynchronous, stack);
        property Component fullEntry:           Qt.createComponent("FullEntryView.qml",     Component.Asynchronous, stack);
        property Component profile:             Qt.createComponent("ProfileView.qml",       Component.Asynchronous, stack);
        property Component calendar:            Qt.createComponent("CalendarView.qml",      Component.Asynchronous, stack);
        property Component users:               Qt.createComponent("UsersView.qml",         Component.Asynchronous, stack);
        property Component loginDialog:         Qt.createComponent("LoginDialog.qml",       Component.Asynchronous, stack);
        property Component trainingProgress:    Qt.createComponent("TrainingProgress.qml",  Component.Asynchronous, stack);
        property Component entryEditor:         Qt.createComponent("EntryEditor.qml",       Component.Asynchronous, stack);
        property Component about:               Qt.createComponent("About.qml",             Component.Asynchronous, stack);
        Connections {
            target: Tasty
            onAuthorizationNeeded: {
                if (!stack.find(function (item) {
                    return item.isLoginDialog;
                }))
                    window.pushLoginDialog();
            }
            onAuthorized: {
                if (stack.currentItem.isLoginDialog)
                    window.popFromStack();
            }
        }
        Connections {
            target: Trainer
            onTrainStarted: {
                if (!stack.find(function (item) {
                    return item.isTrainingProgress;
                }))
                    window.pushTrainingProgress(full);
            }
            onTrainFinished: {
                if (stack.currentItem.isTrainingProgress)
                    window.popFromStack();
            }
        }
    }
    NotificationsView {
        id: notifsView
        anchors.left: parent.left
        anchors.right: parent.right
    }
    Footer {
        id: footer
    }
    InputDialog {
        id: inputDialog
    }
    Dialog {
        id: dialog
        z: 100
        Connections {
            target: Tasty
            onError: {
                dialog.show((code ? code + '\n' : '') + text);
            }
            onInfo: {
                dialog.show(text, true);
            }
        }
    }
}
