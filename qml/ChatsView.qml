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
import QtQuick.Controls 2.2 as Q
import QtQuick.Controls.Material 2.2
import QtQuick.Window 2.2
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
        active: !listView.visible
        emptyString: 'Нет бесед'
    }
    MyListView {
        id: listView
        anchors.fill: parent
        visible: model.size > 0
        model: ChatsModel
        spacing: 2 * mm
        delegate: Rectangle {
            width: window.width
            readonly property int textHeight: lastMessage.y + lastMessage.height - 3 * mm
            height: (textHeight > chatAvatar.height ? textHeight : chatAvatar.height) + 3 * mm
            color: pop.pressed ? Material.primary : 'transparent'
            Component.onCompleted: {
                if (index > ChatsModel.size - 50 && ChatsModel.hasMore)
                    ChatsModel.loadMore();
            }
            MouseArea {
                id: pop
                anchors.fill: parent
                onClicked: {
                    if (model.chat.unreadCount > 0 && Settings.readMessages)
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
                acceptClick: model.chat.type !== Chat.GroupConversation
                url: model.chat.type === Chat.PrivateConversation
                     ? Screen.pixelDensity <= 8 ? user.thumb64 : user.thumb128
                     : model.chat.avatar
                name: model.chat.type === Chat.PrivateConversation
                        ? user.name : model.chat.topic
                onClicked: {
                    window.showChatsOnPop = window.stackSize;
                    if (model.chat.entry)
                        window.pushFullEntry(model.chat.entry)
                    else if (chatAvatar.user)
                        window.pushProfileById(chatAvatar.user.id);
                    back.hide();
                }
            }
            ThemedText {
                id: chatNick
                anchors {
                    top: parent.top
                    left: chatAvatar.right
                }
                width: Math.min(implicitWidth, parent.width - x - date.width - anchors.margins
                                - (recipientStatus.visible ? recipientStatus.width + recipientStatus.anchors.margins * 2
                                                           : anchors.margins))
                text: model.chat.topic
                font.pixelSize: window.fontSmaller
                elide: Text.ElideRight
                wrapMode: Text.NoWrap
                horizontalAlignment: Text.AlignLeft
            }
            Rectangle {
                id: recipientStatus
                anchors {
                    verticalCenter: chatNick.verticalCenter
                    left: chatNick.right
                    margins: 1 * mm
                }
                width: 1 * mm
                height: width
                radius: width / 2
                color: "#4caf50"
                visible: chat.recipient && chat.recipient.isOnline
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
            Loader {
                id: lastMessageAvatar
                anchors {
                    top: chatNick.bottom
                    left: chatAvatar.right
                    margins: 1.5 * mm
                }
                active: !model.chat.isAnonymous && model.chat.lastMessage.userId !== model.chat.recipientId
                         && (model.chat.entry ? model.chat.lastMessage.userId !== model.chat.entry.author.id : true)
                asynchronous: false
                Component.onDestruction: sourceComponent = undefined
                sourceComponent: SmallAvatar {
                    anchors {
                        top: undefined
                        left: undefined
                    }
                    width: 4 * mm
                    height: 4 * mm
                    user: lastMessageAvatar.active ? model.chat.lastMessage.user
                                                   : chatAvatar.user
                }
            }
            ThemedText {
                id: lastMessage
                anchors {
                    top: chatNick.bottom
                    left: lastMessageAvatar.active ? lastMessageAvatar.right : chatAvatar.right
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
        visible: !splash.active
    }
}
