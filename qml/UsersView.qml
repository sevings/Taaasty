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
import QtQuick.Controls 2.2 as Q
import QtQuick.Controls.Material 2.2
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
        model: users.model
        active: !users.visible
        emptyString: 'Список пуст'
    }
    MyListView {
        id: users
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        visible: model.size > 0
        height: contentHeight > parent.height ? parent.height : contentHeight
        model: UsersModel {
            id: usersModel
            mode: back.mode
            tlog: back.tlogId
        }
        delegate: Item {
            width: window.width
            height: usersAvatar.height + 3 * mm
            Component.onCompleted: {
                if (index > usersModel.size - 50 && usersModel.hasMore)
                    usersModel.loadMore();
            }
            Rectangle {
                id: fillRect
                anchors.fill: parent
                color: Material.primary
                visible: usersMouse.pressed
            }
            Poppable {
                id: usersMouse
                anchors.fill: parent
                body: back
                onClicked: {
                    mouse.accepted = true;
                    window.pushTlog(user.id);
                }
            }
            RowLayout {
                anchors {
                    fill: parent
                    margins: 1.5 * mm
                }
                spacing: 1.5 * mm
                SmallAvatar {
                    id: usersAvatar
                    user: model.user
                    popBody: back
                    onClicked: {
                        window.pushProfileById(user.id);
                    }
                }
                Q.Label {
                    id: usersName
                    font.pixelSize: window.fontBigger
                    text: user.name
                    elide: Text.ElideRight
                    Layout.alignment: Qt.AlignVCenter
                    Layout.fillWidth: true
                }
            }
        }
        header: Item {
            width: window.width
            height: usersTitle.paintedHeight + 3 * mm
            Q.Label {
                id: usersTitle
                font.pixelSize: window.fontBigger
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
                y: 1.5 * mm
            }
        }
        footer: ListBusyIndicator {
            running: users.model.loading
            visible: users.model.hasMore
        }
    }
}
