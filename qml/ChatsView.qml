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
import QtQuick.Controls 2.0 as Q
import QtQuick.Controls.Material 2.0
import org.binque.taaasty 1.0

PopupFill {
    id: back
    onStateChanged: {
        if (state == "opened")
            Tasty.reconnectToPusher();
    }
    Splash {
        visible: !listView.visible
        running: listView.model.hasMore
        text: 'Нет бесед'
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
            readonly property int textHeight: lastMessage.y + lastMessage.height - 2 * mm
            height: (textHeight > chatAvatar.height ? textHeight : chatAvatar.height) + 2 * mm
            color: pop.pressed ? Material.primary : 'transparent'
            MouseArea {
                id: pop
                anchors.fill: parent
                onClicked: {
                    if (model.chat.unreadCount > 0)
                        model.chat.readAll();

                    back.hide();
                    window.pushMessages(model.chat);
                }
            }
            SmallAvatar {
                id: chatAvatar
                anchors {
                    top: parent.top
                    margins: 1 * mm
                }
                user: model.chat.entry ? model.chat.entry.author : model.chat.recipient
                acceptClick: !model.chat.isAnonymous && (model.chat.recipient || model.chat.entry)
                onClicked: {
                    back.hide();
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
                font.pointSize: window.fontSmaller
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
                font.pointSize: window.fontSmallest
                color: window.secondaryTextColor
                elide: Text.AlignRight
                wrapMode: Text.NoWrap
            }
            SmallAvatar {
                id: lastMessageAvatar
                anchors {
                    top: chatNick.bottom
                    left: chatAvatar.right
                    margins: 1 * mm
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
                font.pointSize: window.fontSmallest
                color: window.secondaryTextColor
                text: model.chat.lastMessage.truncatedText
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
                visible: model.chat.unreadCount > 0 || model.chat.isMyLastMessageUnread
                Q.Label {
                    visible: model.chat.unreadCount > 0
                    anchors.centerIn: parent
                    text: model.chat.unreadCount
                    font.pointSize: window.fontSmallest
                    color: 'white'
                }
            }
        }
        footer: ListBusyIndicator {
            running: ChatsModel.loading
            visible: ChatsModel.hasMore
        }
    }
}
