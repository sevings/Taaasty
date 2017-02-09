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
        if (state == "opened") {
            Tasty.reconnectToPusher();
            if (window.friendActivity)
                FriendActivityModel.check();
        }
        else if (!window.friendActivity)
            NotifsModel.markAsRead();
    }
    function hideNotifs() {
        back.hide();
        window.showNotifsOnPop = window.stackSize;
    }
    Connections {
        target: window
        onFriendActivityChanged: {
            if (window.friendActivity)
                FriendActivityModel.check();
        }
    }
    Splash {
        id: splash
        model: notifsView.model
        visible: !notifsView.visible
        emptyString: 'Нет уведомлений'
    }
    MyListView {
        id: notifsView
        anchors.fill: parent
        visible: count > 0
        model: window.friendActivity ? FriendActivityModel : NotifsModel
        spacing: 3 * mm
        cacheBuffer: back.visible ? 2 * window.height : 0
        delegate: MouseArea {
            id: notif
            width: window.width
            height: (window.friendActivity ? notifAvatar.height + 4.5 * mm
                                           : Math.max(notifAvatar.height, notifName.height + notifAction.height))
                    + notifContent.height + 1.5 * mm
            onClicked: {
                var fullEntry = model.notification.entry;
                if (fullEntry)
                {
                    back.hideNotifs();
                    window.pushFullEntry(fullEntry);
                }
                else if (model.notification.entityType == 3)//Notification.RelationshipType)
                {
                    back.hideNotifs();
                    
                    var entityUser = model.notification.entityUser;
                    var entityTlogId = entityUser ? entityUser.id
                                                  : model.notification.sender.id;
                    window.pushTlog(entityTlogId);
                }
            }
            Rectangle {
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                    bottom: window.friendActivity ? notifContent.top : parent.bottom
                }
                color: Material.primary
                visible: notif.pressed || window.friendActivity
            }
            SmallAvatar {
                id: notifAvatar
                anchors.margins: 1.5 * mm
                user: model.notification.sender
                acceptClick: back.y <= 0 && model.notification.parentType !== 'AnonymousEntry'
                onClicked: {
                    back.hideNotifs();
                    window.pushProfileById(model.notification.sender.id);
                }
            }
            ThemedText {
                id: notifName
                text: model.notification.sender.name
                anchors {
                    top: parent.top
                    left: notifAvatar.right
                    right: notifDate.left
                    bottomMargin: 0
                }
                font.pixelSize: window.fontSmaller
                elide: Text.ElideRight
                wrapMode: Text.NoWrap
            }
            ThemedText {
                id: notifDate
                anchors {
                    verticalCenter: notifName.verticalCenter
                    right: parent.right
                }
                text: notification.createdAt
                color: window.secondaryTextColor
                font.pixelSize: window.fontSmallest
                wrapMode: Text.NoWrap
            }
            ThemedText {
                id: notifAction
                text: model.notification.actionText
                anchors {
                    top: notifName.bottom
                    left: notifAvatar.right
                    right: unreadNotice.visible ? unreadNotice.left : parent.right
                    topMargin: 0
                }
                font.pixelSize: window.fontSmaller
            }
            Rectangle {
                id: unreadNotice
                anchors {
                    verticalCenter: notifAction.verticalCenter
                    right: parent.right
                    margins: 2 * mm
                }
                width: 1.5 * mm
                height: width
                radius: height / 2
                color: Material.primary
                visible: !model.notification.isRead
            }
            Loader {
                id: notifContent
                anchors {
                    top:   window.friendActivity ? notifAvatar.bottom : notifAction.bottom
                    left:  window.friendActivity ? parent.left        : notifName.left
                    right: window.friendActivity ? parent.right       : notifDate.right
                    topMargin: window.friendActivity ? 1.5 * mm : 0
                    leftMargin: window.friendActivity ? 3 * mm : 0
                    rightMargin: anchors.leftMargin
                }
                asynchronous: false
                Component.onDestruction: sourceComponent = undefined
                readonly property string textContent: window.friendActivity ? '' : notification.text
                readonly property User notificationUser: window.friendActivity && notification.entityType === Notification.RelationshipType
                                                         ? notification.entityUser : null
                readonly property TlogEntry notificationEntry: window.friendActivity && notification.entityType === Notification.EntryType
                                                               ? notification.entry : null
                sourceComponent: {
                    if (!window.friendActivity)
                        notifText;
                    else if (notificationEntry)
                        notifEntry;
                    else if (notificationUser)
                        notifTlog;
                    else
                        undefined;
                }
            }
        }
        footer: ListBusyIndicator {
            running: notifsView.model.loading
            visible: notifsView.model.hasMore
        }
    }
    Component {
        id: notifText
        ThemedText {
            text: textContent
            font.pixelSize: window.fontSmaller
            font.italic: true
        }
    }
    Component {
        id: notifTlog
        Item {
            height: tlogAvatar.height + 3 * mm
            SmallAvatar {
                id: tlogAvatar
                anchors {
                    top: undefined
                    verticalCenter: parent.verticalCenter
                    margins: 1.5 * mm
                }
                user: notificationUser
                onClicked: {
                    back.hideNotifs();
                    window.pushProfileById(user.id);
                }
            }
            Q.Label {
                id: tlogName
                font.pixelSize: window.fontBigger
                text: notificationUser.name
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: tlogAvatar.right
                    right: parent.right
                    leftMargin: 1.5 * mm
                    rightMargin: 1.5 * mm
                }
                elide: Text.ElideRight
            }
        }
    }
    Component {
        id: notifEntry
        TruncatedEntry {
            entry: notificationEntry
            pinVisible: false
            onAvatarClicked: {
                back.hideNotifs();
            }
        }
    }
    Q.BusyIndicator {
        anchors.centerIn: parent
        height: 15 * mm
        width: height
        running: notifsView.model.checking
        visible: !splash.visible
    }
}
