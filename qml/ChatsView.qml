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

import QtQuick 2.8
import QtQuick.Controls 2.1 as Q
import QtQuick.Controls.Material 2.1
import org.binque.taaasty 1.0

PopupFill {
    id: back
    onStateChanged: {
        if (state == "opened")
            Tasty.reconnectToPusher();
    }
    Splash {
        id: splash
        model: ChatsModel
        visible: !listView.visible
        emptyString: 'Нет бесед'
    }
    MyListView {
        id: listView
        anchors.fill: parent
        visible: count > 0
        model: ChatsModel
        spacing: 2 * mm
        cacheBuffer: back.visible ? 2 * window.height : 0
        delegate: Rectangle {
            width: window.width
            readonly property int textHeight: lastMessage.y + lastMessage.height - 3 * mm
            height: (textHeight > chatAvatar.height ? textHeight : chatAvatar.height) + 3 * mm
            color: pop.pressed ? Material.primary : 'transparent'
            MouseArea {
                id: pop
                anchors.fill: parent
                onClicked: {
                    if (model.chat.unreadCount > 0)
                        model.chat.readAll();

                    back.hide();
                    window.showChatsOnPop = window.stackSize;
                    window.pushMessages(model.chat, true);
                }
            }
            SmallAvatar {
                id: chatAvatar
                anchors {
                    top: parent.top
                    margins: 1.5 * mm
                }
                user: model.chat.entry ? model.chat.entry.author : model.chat.recipient
                acceptClick: !model.chat.isAnonymous && (model.chat.recipient || model.chat.entry)
                onClicked: {
                    back.hide();
                    window.showChatsOnPop = window.stackSize;
                    window.pushProfileById(chatAvatar.user.id);
                }
            }
            ThemedText {
                id: chatNick
                anchors {
                    top: parent.top
                    left: chatAvatar.right
                    right: date.left
                }
                text: model.chat.topic
                font.pixelSize: window.fontSmaller
                elide: Text.ElideRight
                wrapMode: Text.NoWrap
                horizontalAlignment: Text.AlignLeft
            }
            ThemedText {
                id: date
                anchors {
                    right: parent.right
                    baseline: chatNick.baseline
                }
                text: model.chat.lastMessage.createdAt
                font.pixelSize: window.fontSmallest
                color: window.secondaryTextColor
                elide: Text.AlignRight
                wrapMode: Text.NoWrap
            }
            SmallAvatar {
                id: lastMessageAvatar
                anchors {
                    top: chatNick.bottom
                    left: chatAvatar.right
                    margins: 1.5 * mm
                }
                user: visible ? model.chat.lastMessage.user
                              : chatAvatar.user
                width: 4 * mm
                height: 4 * mm
                visible: !model.chat.isAnonymous && model.chat.lastMessage.userId !== model.chat.recipientId
                         && (model.chat.entry ? model.chat.lastMessage.userId !== model.chat.entry.author.id : true)
            }
            ThemedText {
                id: lastMessage
                anchors {
                    top: chatNick.bottom
                    left: lastMessageAvatar.visible ? lastMessageAvatar.right : chatAvatar.right
                    right: unreadMessages.visible ? unreadMessages.left : parent.right
                }
                font.pixelSize: window.fontSmallest
                color: window.secondaryTextColor
                text: model.chat.lastMessage.truncatedText || (model.chat.lastMessage.containsImage ? '(изображение)' : '')
                elide: Text.ElideRight
                wrapMode: Text.NoWrap
                textFormat: Text.PlainText
            }
            Rectangle {
                id: unreadMessages
                anchors {
                    verticalCenter: lastMessage.verticalCenter
                    right: parent.right
                    margins: 2 * mm
                }
                width: model.chat.unreadCount > 0 ? 4 * mm : 1.5 * mm
                height: width
                radius: height / 2
                color: Material.primary
                visible: model.chat.unreadCount > 0
                         || model.chat.isMyLastMessageUnread
                         || model.chat.isTyped
                SequentialAnimation
                {
                    running: model.chat.isTyped
                    alwaysRunToEnd : true
                    loops: Animation.Infinite
                    NumberAnimation {
                        target: unreadMessages
                        property: 'opacity'
                        from: 1
                        to: 0
                        duration: 500
                    }
                    NumberAnimation {
                        target: unreadMessages
                        property: 'opacity'
                        from: 0
                        to: 1
                        duration: 500
                    }
                }
            }
            Q.Label {
                visible: model.chat.unreadCount > 0
                anchors.centerIn: unreadMessages
                text: model.chat.unreadCount
                font.pixelSize: window.fontSmallest
                color: 'white'
            }
        }
        footer: ListBusyIndicator {
            running: ChatsModel.loading
            visible: ChatsModel.hasMore
        }
    }
    Q.BusyIndicator {
        anchors.centerIn: parent
        height: 15 * mm
        width: height
        running: ChatsModel.checking
        visible: !splash.visible
    }
}
