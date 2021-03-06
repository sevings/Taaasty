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
        active: !notifsView.visible
        emptyString: 'Нет уведомлений'
    }
    MyListView {
        id: notifsView
        anchors.fill: parent
        visible: model.size > 0
        model: window.friendActivity ? FriendActivityModel : NotifsModel
        spacing: 3 * mm
        delegate: MouseArea {
            id: notif
            width: window.width
            height: (window.friendActivity ? notifAvatar.height + 4.5 * mm
                                           : Math.max(notifAvatar.height, notifName.height + notifAction.height))
                    + notifContent.height + 1.5 * mm
            readonly property Notification thisNotification: model.notification
            onClicked: {
                var fullEntry = thisNotification.entry;
                if (fullEntry)
                {
                    back.hideNotifs();
                    window.pushFullEntry(fullEntry);
                }
                else if (thisNotification.entityType == 3)//Notification.RelationshipType)
                {
                    back.hideNotifs();
                    
                    var entityUser = thisNotification.entityUser;
                    var entityTlogId = entityUser ? entityUser.id
                                                  : thisNotification.sender.id;
                    window.pushTlog(entityTlogId);
                }
            }
            Component.onCompleted: {
                if (index > notifsView.model.size - 50 && notifsView.model.hasMore)
                     notifsView.model.loadMore();
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
                user: thisNotification.sender
                acceptClick: back.y <= 0 && thisNotification.parentType !== 'AnonymousEntry'
                onClicked: {
                    back.hideNotifs();
                    window.pushProfileById(thisNotification.sender.id);
                }
            }
            ThemedText {
                id: notifName
                text: thisNotification.sender.name
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
                text: thisNotification.createdAt
                color: window.secondaryTextColor
                font.pixelSize: window.fontSmallest
                wrapMode: Text.NoWrap
            }
            ThemedText {
                id: notifAction
                text: thisNotification.actionText
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
                visible: !thisNotification.isRead
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
                readonly property string textContent: window.friendActivity ? '' : thisNotification.text
                readonly property User notificationUser: window.friendActivity && thisNotification.entityType === Notification.RelationshipType
                                                         ? thisNotification.entityUser : null
                readonly property TlogEntry notificationEntry: window.friendActivity && thisNotification.entityType === Notification.EntryType
                                                               ? thisNotification.entry : null
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
        visible: !splash.active
    }
}
