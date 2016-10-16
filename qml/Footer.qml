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

Popup {
    id: footer
    height: 48 * sp
    closable: false
    state: "opened"
    MouseArea {
        anchors.fill: parent
        onClicked: {
            mouse.accepted = true;
        }
        onDoubleClicked: {
            mouse.accepted = true;
        }
    }
    IconButton {
        id: left
        anchors {
            margins: 0
            left: parent.left
            top: parent.top
        }
        height: parent.height
        width: parent.width / 4
        icon: '../icons/' + (window.stackSize > 1 || window.notifsShows || window.chatsShows
                             ? 'left-arrow-' : 'menu-rounded-')
              + 'white-128.png'
        onClicked: {
            if (window.notifsShows)
                window.hideNotifs();
            else if (window.chatsShows)
                window.hideChats();
            else if (window.stackSize > 1)
                window.popFromStack();
            else
                forwardAnimation.start();
        }
    }
    IconButton {
        id: chats
        anchors {
            margins: 0
            left: left.right
            top: parent.top
        }
        height: parent.height
        width: parent.width / 4
        visible: Tasty.isAuthorized
        highlighted: window.chatsShows
        icon: '../icons/chat-white-128.png'
        onClicked: {
            window.toggleChats();
        }
        ThemedText {
            anchors {
                left: parent.horizontalCenter
                baseline: parent.verticalCenter
            }
            text: '<b>+' + Tasty.unreadChats + '</b>'
            visible: Tasty.unreadChats > 0
            style: Text.Outline
            color: 'white'
            styleColor: 'black'
            font.pointSize: window.fontBigger
        }
        Rectangle {
            id: chatsNotice
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.top
                margins: 1 * mm
            }
            visible: Tasty.unreadChats
            color: Material.accent
            width: 1.5 * mm
            height: width
            radius: height / 2
        }
    }
    IconButton {
        id: bell
        anchors {
            margins: 0
            left: chats.right
            top: parent.top
        }
        height: parent.height
        width: parent.width / 4
        visible: Tasty.isAuthorized
        highlighted: window.notifsShows
        icon: '../icons/bell-white-128.png'
        onClicked: {
            window.toggleNotifs();
        }
        ThemedText {
            anchors {
                left: parent.horizontalCenter
                baseline: parent.verticalCenter
            }
            text: '<b>+' + Tasty.unreadNotifications + '</b>'
            visible: Tasty.unreadNotifications > 0
            style: Text.Outline
            color: 'white'
            styleColor: 'black'
            font.pointSize: window.fontBigger
        }
        Rectangle {
            id: notifNotice
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.top
                margins: 1 * mm
            }
            visible: Tasty.unreadNotifications
            color: Material.accent
            width: 1.5 * mm
            height: width
            radius: height / 2
        }
    }
    IconButton {
        id: right
        anchors {
            margins: 0
            right: parent.right
            top: parent.top
        }
        height: parent.height
        width: parent.width / 4
        visible: window.canShowPageMenu
        icon: '../icons/menu-white-128.png'
        onClicked: {
            window.showPageMenu();
        }
    }
}
