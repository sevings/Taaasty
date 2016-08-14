import QtQuick 2.7
import QtQuick.Controls 2.0 as Q
import QtQuick.Controls.Material 2.0
//import QtGraphicalEffects 1.0

import org.binque.taaasty 1.0
import ImageCache 2.0

Q.ApplicationWindow {
    id: window
    visible: true
    width: 540
    height: 960
    Material.theme: darkTheme ? Material.Dark : Material.Light
    Material.accent: Material.Green
    Material.primary: Material.BlueGrey
    readonly property bool darkTheme: Settings.darkTheme
    property color backgroundColor: Material.background
//    property color textColor: darkTheme ? 'white' : '#424d52'
    property color secondaryTextColor: darkTheme ? '#9E9E9E' : Qt.darker('#9E9E9E')
    property color greenColor: '#4CAF50'
    property color redColor: '#F44336' // '#f3534b'
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
//    color: backgroundColor
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
            if (!stack.currentItem.isTrainingProgress)
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
    function pushFullEntry(entry, scroll) {
        stack.push(stack.fullEntry,
                   {
                       entry: entry,
                       scrollToBottom: scroll === true,
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
    function pushSettings() {
        stack.push(stack.settings,
                   {
                       poppable: true
                   })
    }
    function pushFlows() {
        stack.push(stack.flows,
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
    function setFeedMode(mode) {
        if (mode !== FeedModel.BetterThanMode)
            stack.currentItem.setMode(mode);
        else
            showLineInput('rating');

        setFooterFromStack();
    }

    Tlog {
        id: emptyTlog
    }
//    BusyBar {
//        id: bar
//        z: 100
//        busy: Tasty.busy > 0
//    }
    Splash {
        id: loadingText
    }
    MainMenu {
        id: menu
        visible: stack.depth === 1 && stack.currentItem && stack.currentItem.x > 0
        onModeChanged: {
            backAnimation.start();
            setFeedMode(mode);
        }
        onTlogRequested: {
            backAnimation.start();
            showLineInput('tlog');
        }
        onChatsRequested: {
            backAnimation.start();
            window.pushChats();
        }
        onFlowsRequested: {
            backAnimation.start();
            window.pushFlows();
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
        onSettingsRequested: {
            backAnimation.start();
            window.pushSettings();
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
//        PropertyAnimation {
//            target: stack.currentItem
//            property: "opacity"
//            to: 1
//            duration: 300
//        }
    }
    Q.StackView {
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
                poppable: Q.StackView.index > 0
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
        property Component settings:            Qt.createComponent("SettingsPage.qml",      Component.Asynchronous, stack);
        property Component flows:               Qt.createComponent("FlowsView.qml",         Component.Asynchronous, stack);
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
    Q.Drawer {
        id: drawer
        edge: Qt.RightEdge
        height: window.height
        width: 40 * mm
        Flickable {
            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                right: parent.right
            }
            height: contentHeight > parent.height ? parent.height : contentHeight
            flickableDirection: Flickable.VerticalFlick
            topMargin: 1 * mm
            bottomMargin: 1 * mm
            contentWidth: parent.width
            contentHeight: column.height
            Column {
                id: column
                width: parent.width
                spacing: 1 * mm

                // BEST SECTION
                MenuItem {
                    text: 'Лучшее'
                    onTriggered: {
                        window.setFeedMode(FeedModel.BestMode);
                        drawer.close();
                    }
                    highlighted: stack.currentItem.mode === FeedModel.BestMode
                    visible: stack.currentItem.bestMode === true
                }
                MenuItem {
                    text: 'Отличное'
                    onTriggered: {
                        window.setFeedMode(FeedModel.ExcellentMode);
                        drawer.close();
                    }
                    highlighted: stack.currentItem.mode === FeedModel.ExcellentMode
                    visible: stack.currentItem.bestMode === true
                }
                MenuItem {
                    text: 'Хорошее'
                    onTriggered: {
                        window.setFeedMode(FeedModel.GoodMode);
                        drawer.close();
                    }
                    highlighted: stack.currentItem.mode === FeedModel.GoodMode
                    visible: stack.currentItem.bestMode === true
                }
                MenuItem {
                    text: 'Неплохое'
                    onTriggered: {
                        window.setFeedMode(FeedModel.WellMode);
                        drawer.close();
                    }
                    highlighted: stack.currentItem.mode === FeedModel.WellMode
                    visible: stack.currentItem.bestMode === true
                }
                MenuItem {
                    text: 'По рейтингу'
                    onTriggered: {
                        window.setFeedMode(FeedModel.BetterThanMode);
                        drawer.close();
                    }
                    highlighted: stack.currentItem.mode === FeedModel.BetterThanMode
                    visible: stack.currentItem.bestMode === true
                }
                MenuSeparator {
                    visible: stack.currentItem.bestMode === true
                }

                // FEED SECTION
                MenuItem {
                    text: 'Поиск'
                    onTriggered: {
                        window.showLineInput('query');
                        drawer.close();
                    }
                    highlighted: visible && stack.currentItem.query.length > 0
                    visible: stack.currentItem.isFeedView === true
                }

                // CHATS SECTION
                MenuItem {
                    text: 'Все беседы'
                    onTriggered: {
                        ChatsModel.mode = 0//ChatsModel.AllChatsMode
                        window.setFooterFromStack();
                        drawer.close();
                    }
                    highlighted: ChatsModel.mode == 0 //= ChatsModel.AllChatsMode
                    visible: stack.currentItem.isChatsView === true
                }
                MenuItem {
                    text: 'Личные'
                    onTriggered: {
                        ChatsModel.mode = 1//ChatsModel.PrivateChatsMode
                        window.setFooterFromStack();
                        drawer.close();
                    }
                    highlighted: ChatsModel.mode == 1 //= ChatsModel.PrivateChatsMode
                    visible: stack.currentItem.isChatsView === true
                }
                MenuItem {
                    text: 'Комментарии'
                    onTriggered: {
                        ChatsModel.mode = 2//ChatsModel.EntryChatsMode
                        window.setFooterFromStack();
                        drawer.close();
                    }
                    highlighted: ChatsModel.mode == 2 //= ChatsModel.EntryChatsMode
                    visible: stack.currentItem.isChatsView === true
                }

                // ENTRY SECTION
                MenuItem {
                    text: stack.currentItem.entry && stack.currentItem.entry.isFavorited
                          ? 'Удалить из избранного' : 'Добавить в избранное'
                    onTriggered: {
                        stack.currentItem.entry.favorite();
                        drawer.close();
                    }
                    visible: stack.currentItem.isFullEntryView === true
                             && stack.currentItem.entry.isFavoritable
                }
                MenuItem {
                    text: 'Открыть в браузере'
                    onTriggered: {
                        var url = stack.currentItem.entry.url;
                        Qt.openUrlExternally(url);
                        drawer.close();
                    }
                    visible: stack.currentItem.isFullEntryView === true
                             && stack.currentItem.entry.url.length > 0
                }
                MenuItem {
                    text: ((stack.currentItem.isFullEntryView === true
                            && stack.currentItem.entry.chat.type === Chat.PrivateConversation)
                           || (stack.currentItem.isMessagesView === true
                               && stack.currentItem.chat.type === Chat.PrivateConversation))
                          ? 'Удалить переписку' : 'Покинуть беседу'
                    onTriggered: {
                        if (stack.currentItem.isFullEntryView)
                            stack.currentItem.entry.chat.remove();
                        else
                            stack.currentItem.chat.remove();

                        drawer.close();
                    }
                    visible: (stack.currentItem.isFullEntryView === true
                              && stack.currentItem.entry.chat.isInvolved)
                             || (stack.currentItem.isMessagesView === true
                                 && stack.currentItem.chat.isInvolved)
                }

                // FLOWS SECTION
                MenuItem {
                    text: 'Все потоки'
                    onTriggered: {
                        stack.currentItem.mode = 0; //= FlowsModel.AllFlowsMode
                        window.setFooterFromStack();
                        drawer.close();
                    }
                    highlighted: stack.currentItem.mode === FlowsModel.AllFlowsMode
                    visible: stack.currentItem.isFlowsView === true
                }
                MenuItem {
                    text: 'Мои потоки'
                    onTriggered: {
                        stack.currentItem.mode = 1; //= FlowsModel.MyFlowsMode
                        window.setFooterFromStack();
                        drawer.close();
                    }
                    highlighted: stack.currentItem.mode === FlowsModel.MyFlowsMode
                    visible: stack.currentItem.isFlowsView === true
                }

                // PROFILE SECTION
                MenuItem {
                    text: 'Обзор постов'
                    onTriggered: {
                        window.pushCalendar(stack.currentItem.author.id);
                        drawer.close();
                    }
                    highlighted: stack.currentItem.mode === FlowsModel.AllFlowsMode
                    visible: stack.currentItem.isProfileView === true
                             && (!stack.currentItem.author.isPrivacy
                                 || stack.currentItem.tlog.myRelationship === Tlog.Friend
                                 || stack.currentItem.tlog.myRelationship === Tlog.Me)
                }
            }
        }
    }
    Footer {
        id: footer
    }
    InputDialog {
        id: inputDialog
        anchors.margins: 2 * mm
    }
//    DropShadow {
//        anchors.fill: inputDialog
//        horizontalOffset: 0.6 * mm
//        verticalOffset: 0.6 * mm
//        samples: 1
//        color: "#80000000"
//        source: inputDialog
//        cached: true
//    }
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
