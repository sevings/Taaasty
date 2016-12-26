/*
 * Copyright (C) 2016 Vasily Khodakov
 * Contact: <binque@ya.ru>
 *
 * This file is part of Taaasty.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
    property color secondaryTextColor: darkTheme ? '#b2ffffff' : '#89000000'
    property color greenColor: '#4CAF50'
    property color redColor: '#F44336' // '#f3534b'
    property int fontBiggest:  34 * Settings.fontZoom / 100 //30
    property int fontBigger:   20 * Settings.fontZoom / 100 //25
    property int fontNormal:   16 * Settings.fontZoom / 100 //20
    property int fontSmaller:  14 * Settings.fontZoom / 100 //17
    property int fontSmallest: 12 * Settings.fontZoom / 100 //14
    readonly property bool notifsShows: notifsView && notifsView.item && notifsView.item.showing
    readonly property bool chatsShows: chatsView && chatsView.item && chatsView.item.showing
    readonly property int stackSize: stack ? stack.depth : 0
    readonly property int footerY: footer.y
    readonly property bool canShowPageMenu: !pageMenu.autoclose
    property bool friendActivity: false
    property int showChatsOnPop: 0
    property int showNotifsOnPop: 0
    property CachedImage savingImage

    property StackView stack: stackLoader.item

    title: qsTr("Taaasty")
    color: stackLoader.opacity == 1 ? window.backgroundColor : '#ff000000'
    onWidthChanged: {
        Tasty.setImageWidth(window.width - 3 * mm, window.width - 12.5 * mm);
    }
    Component.onCompleted: {
        Tasty.setImageWidth(window.width - 3 * mm, window.width - 12.5 * mm);
    }
    onClosing: {
        close.accepted = mayBeClose();
    }
    function mayBeClose() {
        if (notifsShows) {
            hideNotifs();
            return false;
        }
        if (chatsShows) {
            hideChats();
            return false;
        }
        if (stack.depth > 1) {
            if (!stack.currentItem.isTrainingProgress)
                popFromStack();
            return false;
        }
        if (!menu.visible) {
            forwardAnimation.start();
            return false;
        }
        return true;
    }
    function showMessage(txt, info, permanent) {
        dialog.show(txt, info, permanent);
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
        if (mode === 'query')
            inputDialog.text = stack.currentItem.query || ''
    }
    function hideLineInput() {
        inputDialog.state = "closed";
        inputDialog.clear();
    }
    function openLink(url) {
        var matches = /taaasty.com\/(?:~|%7E)?([^\/]+)(?:\/([\d]+))?/.exec(url);
        if (!matches)
            matches = /([a-zA-Z0-9_\\-\\.]+).mmm-tasty.ru(?:\/entries\/([\d]+))?/.exec(url);
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
        var item = stack.find(function (item) {
            return item.isMessagesView && item.chat.id === chat.id;
        })
        if (item) {
            if (item !== stack.currentItem)
                stack.pop(item);
            return;
        }

        stack.push(stack.messages,
                   {
                       chat: chat,
                       poppable: true
                   }
                   )
    }
    function pushFullEntry(entry, toBottom, toFirst, feedModel) {
        var item = stack.find(function (item) {
            return item.isFullEntryView && item.entry.id === entry.id;
        })
        if (item) {
            if (item !== stack.currentItem)
                stack.pop(item);
            return;
        }

        stack.push(stack.fullEntry,
                   {
                       entry: entry,
                       scrollToBottom: toBottom === true,
                       scrollToFirst: toFirst === true,
                       feedModel: feedModel,
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
    function pushTags(tlog) {
        stack.push(stack.tags,
                   {
                       tlog: tlog,
                       tlogId: tlog.id,
                       poppable: true
                   }
                   )
    }
    function pushTlogTag(tlog, tag) {
        stack.push(stack.feed,
                   {
                       mode: FeedModel.TlogMode,
                       tlog: tlog,
                       tlogId: tlog.id,
                       slug: tlog.slug,
                       tag: tag,
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
    function simplePush(comp, pop) {
        stack.push(comp,
                   {
                       poppable: pop !== false
                   })
    }
    function simpleReplace(comp, pop) {
        stack.replace(comp,
                   {
                       poppable: pop !== false
                   })
    }
    function pushLoginDialog() {
        simplePush(stack.loginDialog);
    }
    function pushHello() {
        simplePush(stack.hello);
    }
    function pushTrainingProgress() {
        simplePush(stack.trainingProgress, false);
    }
    function pushHelp() {
        simplePush(stack.help);
    }
    function pushAbout() {
        simplePush(stack.about);
    }
    function pushSettings() {
        simplePush(stack.settings);
    }
    function pushFlows() {
        simplePush(stack.flows);
    }
    function pushEntryEditor() {
        simplePush(stack.entryEditor);
    }
    function popFromStack() {
        stack.pop();

        if (stackSize === showChatsOnPop)
        {
            showChats();
            showChatsOnPop = 0;
        }
        else if (stackSize === showNotifsOnPop)
        {
            showNotifs();
            showNotifsOnPop = 0;
        }
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
    PropertyAnimation {
        id: backAnimation
        target: stack.currentItem
        property: "x"
        to: 0
        duration: 100
    }
    PropertyAnimation {
        id: forwardAnimation
        target: stack.currentItem
        property: "x"
        to: 40 * mm
        duration: 100
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
            property Component hello:               Qt.createComponent("Hello.qml",             Component.Asynchronous, stack)
            property Component trainingProgress:    Qt.createComponent("TrainingProgress.qml",  Component.Asynchronous, stack)
            property Component entryEditor:         Qt.createComponent("EntryEditor.qml",       Component.Asynchronous, stack)
            property Component help:                Qt.createComponent("HelpPage.qml",          Component.Asynchronous, stack)
            property Component about:               Qt.createComponent("About.qml",             Component.Asynchronous, stack)
            property Component settings:            Qt.createComponent("SettingsPage.qml",      Component.Asynchronous, stack)
            property Component flows:               Qt.createComponent("FlowsView.qml",         Component.Asynchronous, stack)
            property Component tags:                Qt.createComponent("TagsView.qml",          Component.Asynchronous, stack)
            Connections {
                target: Tasty
                onAuthorizationNeeded: {
                    if (!stack.find(function (item) {
                        return item.isLoginDialog;
                    }))
                        window.pushLoginDialog();
                }
                onAuthorizedChanged: {
                    if (stack.currentItem.isLoginDialog)
                        window.popFromStack();//window.simpleReplace(stack.hello);
                }
            }
            Connections {
                target: Trainer
                onTrainStarted: {
                    if (!stack.find(function (item) {
                        return item.isTrainingProgress;
                    }))
                        window.pushTrainingProgress();
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
        openable: !menu.visible
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
            onNetworkAccessible: {
                if (dialog.permanent)
                    dialog.close();
            }
            onNetworkNotAccessible: {
                dialog.show('Сеть недоступна', false)
            }
        }
    }
}
