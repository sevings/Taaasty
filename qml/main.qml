import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
//import QtGraphicalEffects 1.0

import org.binque.taaasty 1.0
import ImageCache 2.0

ApplicationWindow {
    id: window
    visible: false
    width: 540
    height: 960
    Material.theme: darkTheme ? Material.Dark : Material.Light
    Material.accent: Material.DeepOrange
    Material.primary: Material.BlueGrey
    readonly property bool darkTheme: Settings.darkTheme
    property color backgroundColor: Material.background
    property color secondaryTextColor: darkTheme ? '#9E9E9E' : Qt.darker('#9E9E9E')
    property color greenColor: '#4CAF50'
    property color redColor: '#F44336' // '#f3534b'
    property int fontBiggest: 30
    property int fontBigger: 25
    property int fontNormal: 20
    property int fontSmaller: 17
    property int fontSmallest: 14
    readonly property bool notifsShows: notifsView && notifsView.item && notifsView.item.showing
    readonly property bool chatsShows: chatsView && chatsView.item && chatsView.item.showing
    readonly property int anonymousId: 4409
    readonly property int stackSize: stack ? stack.depth : 0
    readonly property bool canShowPageMenu: pageMenu && !pageMenu.autoclose
    property CachedImage savingImage

    property StackView stack: stackLoader.item

    title: qsTr("Taaasty")
    color: stackLoader.opacity == 1 ? window.backgroundColor : '#ff000000'
    onWidthChanged: {
        Tasty.setImageWidth(window.width - 2 * mm, window.width - 11 * mm);
    }
    Component.onCompleted: {
        Tasty.setImageWidth(window.width - 2 * mm, window.width - 11 * mm);
    }
    onClosing: {
        if (notifsShows) {
            hideNotifs();
            close.accepted = false;
        }
        else if (chatsShows) {
            hideChats();
            close.accepted = false;
        }
        else if (stack.depth > 1) {
            if (!stack.currentItem.isTrainingProgress)
                stack.pop();
            close.accepted = false;
        }
    }
    function showNotifs() {
        notifsView.item.show();
    }
    function hideNotifs() {
        notifsView.item.hide();
    }
    function toggleNotifs() {
        if (chatsShows)
            hideChats();
        notifsView.item.toggle();
    }
    function showChats() {
        chatsView.item.show();
    }
    function hideChats() {
        chatsView.item.hide();
    }
    function toggleChats() {
        if (notifsShows)
            hideNotifs();
        chatsView.item.toggle();
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
    function pushProfile(tlog) {
        stack.push(stack.profile,
                   {
                       tlog: tlog,
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
    function pushTlog(tlogId, sinceId, sinceDate, mode) {
        if (mode === undefined)
            mode = FeedModel.TlogMode;

        if (stack.currentItem.isFeedView
                && stack.currentItem.mode === mode
                && stack.currentItem.tlogId === tlogId)
            return;

        stack.push(stack.feed,
                   {
                       mode: mode,
                       tlogId: tlogId,
                       sinceId: sinceId || 0,
                       sinceDate: sinceDate || '',
                       poppable: true
                   }
                   )
    }
    function pushCalendar(tlog) {
        stack.push(stack.calendar,
                   {
                       tlog: tlog,
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
    }
    function setFeedMode(mode) {
        if (mode !== FeedModel.BetterThanMode)
            stack.currentItem.setMode(mode);
        else
            showLineInput('rating');
    }
    function showPageMenu() {
        pageMenu.open();
    }
    MainMenu {
        id: menu
        visible: stackLoader.opacity == 1
                 && stack.depth === 1 && stack.currentItem && stack.currentItem.x > 0
    }
    ParallelAnimation {
        id: backAnimation
        PropertyAnimation {
            target: stack.currentItem
            property: "x"
            to: 0
            duration: 100
        }
    }
    ParallelAnimation {
        id: forwardAnimation
        PropertyAnimation {
            target: stack.currentItem
            property: "x"
            to: 40 * mm
            duration: 100
        }
    }
    Loader {
        id: stackLoader
        anchors.fill: parent
        asynchronous: true
        focus: true
        opacity: 0
        Behavior on opacity {
            NumberAnimation {
                duration: 200
                easing.type: Easing.OutQuad;
            }
        }
        sourceComponent: StackView {
            id: stack
            anchors.fill: parent
            initialItem: feed
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
            property Component feed:                Qt.createComponent("FeedView.qml",     Component.PreferSynchronous, stack)
            property Component messages:            Qt.createComponent("MessagesView.qml",      Component.Asynchronous, stack)
            property Component fullEntry:           Qt.createComponent("FullEntryView.qml",     Component.Asynchronous, stack)
            property Component profile:             Qt.createComponent("ProfileView.qml",       Component.Asynchronous, stack)
            property Component calendar:            Qt.createComponent("CalendarView.qml",      Component.Asynchronous, stack)
            property Component users:               Qt.createComponent("UsersView.qml",         Component.Asynchronous, stack)
            property Component loginDialog:         Qt.createComponent("LoginDialog.qml",       Component.Asynchronous, stack)
            property Component trainingProgress:    Qt.createComponent("TrainingProgress.qml",  Component.Asynchronous, stack)
            property Component entryEditor:         Qt.createComponent("EntryEditor.qml",       Component.Asynchronous, stack)
            property Component about:               Qt.createComponent("About.qml",             Component.Asynchronous, stack)
            property Component settings:            Qt.createComponent("SettingsPage.qml",      Component.Asynchronous, stack)
            property Component flows:               Qt.createComponent("FlowsView.qml",         Component.Asynchronous, stack)
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
            Component.onCompleted: {
                window.visible = true;
                stackLoader.opacity = 1;
            }
        }
    }
    Loader {
        id: notifsView
        asynchronous: true
        width: window.width
        height: window.height
        sourceComponent: NotificationsView { }
    }
    Loader {
        id: chatsView
        asynchronous: true
        width: window.width
        height: window.height
        sourceComponent: ChatsView { }
    }
    PageMenu {
        id: pageMenu
        page: stack.currentItem
        openable: !menu.visible && !notifsShows
    }
    Footer {
        id: footer
    }
    InputDialog {
        id: inputDialog
        anchors.margins: 2 * mm
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
