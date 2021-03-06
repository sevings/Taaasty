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

import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2
import org.binque.taaasty 1.0

Pane {
    id: back
    innerFlick: listView
    hasMenu: true
    color: window.darkTheme ? window.backgroundColor : '#e6f1f2'
    property Chat chat: Chat { }
    property Tlog tlog: Tlog {
        tlogId: chat.recipientId
    }
    readonly property bool isMessagesView: true
    property bool wasEmpty
    signal addGreeting(string slug)
    Component.onCompleted: reload()
    function reload() {
        back.wasEmpty = !listView.visible;
        chat.messages.check();
        listView.positionViewAtEnd();
        listView.atYBegin = listView.atYBeginning;

        if (chat.recipient)
            chat.recipient.checkStatus();

        chat.syncRead();
    }
    Connections {
        target: chat
        onLoadingChanged: {
            if (!chat.loading)
                chat.messages.check();
        }
        onMessageSent: {
            listView.positionViewAtEnd();
        }
        onLeft: {
            window.popFromStack();
        }
    }
    Poppable {
        body: back
    }
    Splash {
        id: splash
        model: chat.messages
        active: !listView.visible || !chat.messages.size
        running: !listView.visible && (chat.loading || chat.messages.loading
                                       || (!chat.messages.size && chat.messages.checking))
        emptyString: 'Нет сообщений'
    }
    MyListView {
        id: listView
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        height: Math.min(contentHeight, parent.height)
        visible: !chat.loading && (model.size > 0 || !model.hasMore)
        model: chat.messages
        readonly property int headerHeight: headerItem.visibleHeight
        property bool atYBegin
        onCountChanged: {
            if (back.wasEmpty) {
                positionViewAtEnd();
                back.wasEmpty = false;
            }
        }
        Connections {
            target: chat.messages
            onItemsAboutToBePrepended: {
                if (listView.atYBegin === undefined)
                    return;

                listView.atYBegin = listView.indexAt(0, listView.contentY) <= 0;
            }
            onItemsPrepended: {
                if (!listView.atYBegin)
                    return;

                listView.positionViewAtIndex(prependedCount, ListView.Beginning);
                listView.contentY -= listView.headerHeight;
                listView.returnToBounds();
            }
        }
        header: ListBusyIndicator {
            running: listView.model.loading
            visible: listView.model.hasMore
            footer: false
        }
        delegate: Item {
            width: window.width
            height: Math.max(messageAvatar.height, messageBack.height) + 2 * mm
            readonly property Message thisMessage: model.message
            readonly property bool isMyMessage: chat.userId === thisMessage.userId
            Poppable {
                body: back
                onClicked: {
                    menu.show(thisMessage, isMyMessage);
                }
            }
            Component.onCompleted: {
                if (!thisMessage.isRead && Settings.readMessages)
                    thisMessage.read();

                if (index < 50 && listView.model.hasMore)
                    listView.model.loadMore();
            }
            states: [
                State {
                    when: isMyMessage
                    AnchorChanges {
                        target: messageAvatar
                        anchors {
                            left: undefined
                            right: parent.right
                        }
                    }
                    AnchorChanges {
                        target: messageBack
                        anchors {
                            left: undefined
                            right: messageAvatar.left
                        }
                    }
                },
                State {
                    when: !isMyMessage
                    AnchorChanges {
                        target: messageAvatar
                        anchors {
                            left: parent.left
                            right: undefined
                        }
                    }
                    AnchorChanges {
                        target: messageBack
                        anchors {
                            left: messageAvatar.right
                            right: undefined
                        }
                    }
                }
            ]
            SmallAvatar {
                id: messageAvatar
                anchors {
                    top: parent.top
                    margins: 1.5 * mm
                }
                width: 6.5 * mm
                user: thisMessage.user
                popBody: back
                acceptClick: !chat.isAnonymous
                onClicked: {
                    window.pushProfileById(thisMessage.user.id);
                }
            }
            Rectangle {
                id: messageBack
                anchors {
                    top: messageAvatar.top
                }
                readonly property int maxWidth: window.width - messageAvatar.width - 3 * mm
                readonly property int dateWidth: messageDate.contentWidth
                                                 + (unreadMessage.visible ? unreadMessage.width + 1.5 * mm : 0)
                readonly property int textWidth: Math.max(messageText.contentWidth, dateWidth) + 3 * mm
                width: textWidth > maxWidth || messageImages.active ? maxWidth : textWidth
                height: messageDate.y + messageDate.contentHeight + 1.5 * mm
                color: window.darkTheme ? '#404040' : window.backgroundColor
                Loader {
                    id: messageImages
                    anchors {
                        top: parent.top
                        left: parent.left
                        right: parent.right
                        margins: 1.5 * mm
                    }
                    height: item ? item.height : - 1.5 * mm
                    asynchronous: false
                    Component.onDestruction: sourceComponent = undefined
                    readonly property AttachedImagesModel imagesModel: thisMessage.attachedImagesModel
                    active: imagesModel && imagesModel.rowCount() > 0
                    sourceComponent: ListView {
                        interactive: false
                        spacing: 1.5 * mm
                        height: contentHeight
                        model: messageImages.imagesModel
                        delegate: MyImage {
                            id: picture
                            width: messageImages.width
                            height: image.height / image.width * width
                            url: image.url
                            extension: image.type
                            savable: true
                            popBody: back
                        }
                    }
                }
                ThemedText {
                    id: messageText
                    anchors {
                        top: messageImages.bottom
                        left: parent.left
                    }
                    width: messageBack.maxWidth - 3 * mm
                    font.pixelSize: window.fontSmaller
                    text: (thisMessage.replyTo && thisMessage.replyTo.name.length > 0
                           ? '<b>' + thisMessage.replyTo.name + '</b>, ' : '') + thisMessage.text
                    textFormat: thisMessage.containsImage ? Text.RichText : Text.StyledText
                    onLinkActivated: window.openLink(link)
                    height: text.length > 0 ? contentHeight : -1.5 * mm
                }
                ThemedText {
                    id: messageDate
                    anchors {
                        top: messageText.bottom
                        left: parent.left
                    }
                    width: messageBack.maxWidth - 3 * mm - (unreadMessage.visible ? unreadMessage.width + 1.5 * mm : 0)
                    font.pixelSize: window.fontSmallest
                    text: (chat.type == Chat.PrivateConversation || !thisMessage.user.name.length
                           ? '' : thisMessage.user.name + ', ')
                          + thisMessage.createdAt
                    color: window.secondaryTextColor
                }
                Rectangle {
                    id: unreadMessage
                    anchors {
                        verticalCenter: messageDate.verticalCenter
                        right: parent.right
                        margins: 1.5 * mm
                    }
                    width: 1.5 * mm
                    height: width
                    radius: height / 2
                    color: Material.primary
                    visible: !thisMessage.isRead
                }
                Rectangle {
                    anchors.fill: parent
                    color: '#80808080'
                    visible: thisMessage.loading
                }
            }
        }
        footer: Item {
            height: typedText.height + messageEditor.height + 1.5 * mm
            width: window.width
            z: chat.messages.size + 10
            onHeightChanged: {
                if (messageEditor.activeFocus)
                    listView.positionViewAtEnd();
            }
            ThemedText {
                id: typedText
                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                }
                text: chat.isTyped ? chat.typedUsers
                                   : chat.type === Chat.PrivateConversation
                                     ? chat.recipient.lastSeenAt : ''
                font.pixelSize: window.fontSmallest
                color: window.secondaryTextColor
                horizontalAlignment: chat.isTyped ? Text.AlignLeft : Text.AlignHCenter
            }
            Rectangle {
                anchors.fill: messageEditor
                visible: splash.visible
                color: back.color
            }
            MessageEditor {
                id: messageEditor
                anchors {
                    left: parent.left
                    right: parent.right
                    top: typedText.bottom
                }
                uploading: chat.sending
                attach: chat.images.size
                popBody: back
                visible: chat.canTalk
                height: visible ? implicitHeight : - 1.5 * mm
                onSent: {
                    chat.sendMessage(messageEditor.message)
                }
                onMessageChanged: {
                    if (message.length > 0)
                        chat.sendTyped()
                }
                onActiveFocusChanged: {
                    if (focus)
                        listView.positionViewAtEnd();
                }
                Connections {
                    target: chat
                    onMessageSent: {
                        messageEditor.clear();
                    }
                }
                Connections {
                    target: back
                    onAddGreeting: {
                        messageEditor.addGreeting(slug);
                    }
                    onHeightChanged: { // keyboard shown
                        if (!messageEditor.focus)
                            return;

                        if (!chat.messages.size) {
                            listView.contentY = listView.contentHeight;
                            listView.returnToBounds();
                        }
                        else
                            listView.positionViewAtEnd();
                    }
                }
            }
        }
    }
    Popup {
        id: menu
        height: menuColumn.height + 3 * mm
        anchors.margins: 2 * mm
        property Message message
        property bool isMyMessage: false
        function close() {
            state = "closed";
        }
        function show(msg, my) {
            if (msg.loading || !Tasty.isAuthorized)
                return;

            menu.message = msg;
            menu.isMyMessage = my;
            window.hideFooter();
            state = "opened";
        }
        onClosing: menu.close()
        Column {
            id: menuColumn
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                topMargin: 1.5 * mm
                bottomMargin: 1.5 * mm
            }
            spacing: 1.5 * mm
            MenuItem {
                text: 'Ответить'
                onTriggered: {
                    addGreeting(menu.message.user.slug);
                    menu.close();
                    listView.positionViewAtEnd();
                }
            }
            MenuItem {
                visible: menu.message && !menu.message.isRead
                         && !menu.isMyMessage
                text: 'Прочитано'
                onTriggered: {
                    chat.readTo(menu.message.id);
                    menu.close();
                }
            }
            MenuItem {
                text: 'Удалить'
                onTriggered: {
                    window.askUser('Удалить сообщение? Другие пользователи все равно будут его видеть.',
                                   function() {
                                       menu.message.remove();
                                   });
                    menu.close();
                }
            }
            MenuItem {
                visible: menu.isMyMessage
//                         && Tasty.me && Tasty.me.isPremium
                text: 'Удалить для всех'
                onTriggered: {
                    window.askUser('Удалить сообщение для всех пользователей?',
                                   function() {
                                       menu.message.remove(true);
                                   });
                    menu.close();
                }
            }
        }
    }
}
