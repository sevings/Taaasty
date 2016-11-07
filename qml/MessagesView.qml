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
import QtQuick.Controls.Material 2.0
import org.binque.taaasty 1.0

Pane {
    id: back
    innerFlick: listView
    color: window.darkTheme ? window.backgroundColor : '#e6f1f2'
    property Chat chat: Chat { }
    property Tlog tlog: Tlog {
        tlogId: chat.recipientId
    }
    readonly property bool isMessagesView: true
    property bool wasEmpty
    signal addGreeting(string slug)
    Component.onCompleted: {
        back.wasEmpty = !listView.visible;
        chat.messages.check();
        listView.positionViewAtEnd();
        listView.atYBegin = listView.atYBeginning;
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
    }
    Poppable {
        body: back
    }
    Splash {
        visible: !listView.visible || !listView.count
        running: !listView.visible && (chat.loading || chat.messages.loading
                                       || (!listView.count && chat.messages.checking))
        text: chat.messages.errorString || 'Нет сообщений'
    }
    MyListView {
        id: listView
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        height: contentHeight > parent.height ? parent.height : contentHeight
        visible: !chat.loading && (count > 0 || !model.hasMore)
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
            height: (messageAvatar.height > messageBack.height
                     ? messageAvatar.height : messageBack.height) + 2 * mm
            readonly property bool isMyMessage: chat.userId === message.userId 
            Poppable {
                body: back
                onClicked: {
                    menu.show(message);
                }
            }
            Component.onCompleted: {
                if (!message.isRead)
                    message.read();

                if (index < 10)
                    listView.model.loadMore();
            }
            SmallAvatar {
                id: messageAvatar
                anchors {
                    top: parent.top
                    margins: 1.5 * mm
                    left: isMyMessage ? undefined : parent.left
                    right: isMyMessage ? parent.right : undefined
                }
                width: 6.5 * mm
                user: message.user
                popBody: back
                acceptClick: !chat.isAnonymous
                onClicked: {
                    window.pushProfileById(message.user.id);
                }
            }
            Rectangle {
                id: messageBack
                anchors {
                    top: parent.top
                    left: isMyMessage ? undefined : messageAvatar.right
                    right: isMyMessage ? messageAvatar.left : undefined
                    margins: 1.5 * mm
                    leftMargin: isMyMessage ? 1.5 * mm : 0
                    rightMargin: isMyMessage ? 0 : 1.5 * mm
                }                
                readonly property int maxWidth: window.width - messageAvatar.width - 3 * mm
                readonly property int dateWidth: messageDate.contentWidth
                                                 + (unreadMessage.visible ? unreadMessage.width + 1.5 * mm : 0)
                readonly property int textWidth: Math.max(messageText.contentWidth, dateWidth) + 3 * mm
                width: textWidth > maxWidth || messageImages.visible ? maxWidth : textWidth
                height: messageDate.y + messageDate.contentHeight + 1.5 * mm
                color: window.darkTheme ? '#404040' : window.backgroundColor
                ListView {
                    id: messageImages
                    anchors {
                        top: parent.top
                        left: parent.left
                        right: parent.right
                        margins: 1.5 * mm
                    }
                    interactive: false
                    spacing: 1.5 * mm
                    property AttachedImagesModel imagesModel: message.attachedImagesModel
                    height: visible ? (imagesModel.listRatio() * width
                            + (count - 1) * mm) : -1.5 * mm
                    model: imagesModel
                    visible: imagesModel && count > 0
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
                ThemedText {
                    id: messageText
                    anchors {
                        top: messageImages.bottom
                        left: parent.left
                    }
                    width: messageBack.maxWidth - 3 * mm
                    font.pointSize: window.fontSmaller
                    text: (message.replyTo && message.replyTo.name.length > 0 ? '<b>' + message.replyTo.name + '</b>, ' : '') + message.text
                    textFormat: message.containsImage ? Text.RichText : Text.StyledText
                    onLinkActivated: window.openLink(link)
                    height: message.text.length > 0 ? contentHeight : -1.5 * mm
                }
                ThemedText {
                    id: messageDate
                    anchors {
                        top: messageText.bottom
                        left: parent.left
                    }
                    width: messageBack.maxWidth - 3 * mm - (unreadMessage.visible ? unreadMessage.width + 1.5 * mm : 0)
                    font.pointSize: window.fontSmallest
                    text: (chat.type == Chat.PrivateConversation || !message.user.name.length
                           ? '' : message.user.name + ', ')
                          + message.createdAt
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
                    visible: !message.isRead
                }
            }
        }
        footer: Item {
            height: typedText.height + messageEditor.height + 1.5 * mm
            width: window.width
            z: listView.count + 10
            ThemedText {
                id: typedText
                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                }
                text: chat.typedUsers
                font.pointSize: window.fontSmallest
                color: window.secondaryTextColor
            }
            MessageEditor {
                id: messageEditor
                anchors {
                    left: parent.left
                    right: parent.right
                    top: typedText.bottom
                }
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
                    onSendingMessageError: {
                        messageEditor.uploading = false;
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

                        if (!listView.count) {
                            listView.contentY = listView.contentHeight;
                            listView.returnToBounds();
                        }
                        else
                            listView.positionViewAtEnd();
                    }
                }
                Connections {
                    target: listView
                    onMovingVerticallyChanged: messageEditor.hideMenu()
                }
                Connections {
                    target: menu
                    onOpened: messageEditor.hideMenu()
                }
            }
        }
    }
    Popup {
        id: menu
        height: menuColumn.height + 2 * mm
        anchors.margins: 2 * mm
        property Message message: Message { }
        function close() {
            state = "closed";
        }
        function show(msg) {
            menu.message = msg;
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
        }
    }
}
