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

Pane {
    id: back
    innerFlick: users
    property int mode: UsersModel.MyFollowingsMode
    property int tlogId
    property Tlog tlog: Tlog {
        tlogId: back.tlogId
    }
    Poppable {
        body: back
    }
    Splash {
        visible: !users.visible
        running: users.model.hasMore
        text: 'Список пуст'
    }
    MyListView {
        id: users
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        visible: count > 0
//        interactive: back.x == 0
        height: contentHeight > parent.height ? parent.height : contentHeight
        model: UsersModelTlog {
            mode: back.mode
            tlog: back.tlogId
        }
        delegate: Item {
            width: window.width
            height: usersAvatar.height + 2 * mm
            Rectangle {
                id: fillRect
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
                }
                width: parent.width
                color: Material.primary
                visible: usersMouse.pressed || removing.running
            }
            Poppable {
                id: usersMouse
                anchors.fill: parent
                body: back
                onClicked: {
                    mouse.accepted = true;
                    window.pushTlog(user.id);
                    removing.stop();
                }
            }
            SmallAvatar {
                id: usersAvatar
                anchors {
                    top: undefined
                    verticalCenter: parent.verticalCenter
                    margins: 1 * mm
                }
                user: model.user
                popBody: back
                onClicked: {
                    window.pushProfileById(user.id);
                }
            }
            Q.Label {
                id: usersName
                font.pointSize: window.fontBigger
                text: user.name
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: usersAvatar.right
                    right: parent.right
                    leftMargin: 1 * mm
                    rightMargin: 1 * mm
                }
                elide: Text.ElideRight
            }
            NumberAnimation {
                id: removing
                target: fillRect
                property: "width"
                easing.type: Easing.OutQuad
                from: window.width
                to: 0
                duration: 4000
                onStopped: {
                    if (fillRect.width > 0)
                        fillRect.width = window.width;
                    else
                        users.model.removeUser(user.id);
                }
            }
        }
        header: Item {
            width: window.width
            height: usersTitle.paintedHeight + 2 * mm
            Q.Label {
                id: usersTitle
                font.pointSize: window.fontBigger
                text: {
                    if (back.mode === UsersModel.FollowersMode)
                        'Подписчики';
                    else if (back.mode === UsersModel.FollowingsMode)
                        'Подписки';
                    else if (back.mode === UsersModel.MyIgnoredMode)
                        'Заблокированы';
                    else
                        '';
                }
                anchors.centerIn: parent
                y: 1 * mm
            }
        }
        footer: ListBusyIndicator {
            running: users.model.loading
            visible: users.model.hasMore
        }
    }
}
