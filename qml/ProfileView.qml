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
    id: profileView
    innerFlick: flickable
    property int tlogId
    property string slug: ''
    property Tlog tlog: Tlog {
        tlogId: profileView.tlogId
        slug: profileView.slug
    }
    property Author author: tlog.author
    readonly property bool isProfileView: true
    Splash {
        visible: !column.visible
        running: tlog.loading
        text: tlog.errorString
    }
    Component.onCompleted: {
        if (!tlog.slug.length)
        {
            tlog.reload();
            if (tlog.tlogId !== author.id)
                author.reload();
        }
        else if (tlog.tlogId && tlog.slug)
            tlog.author.checkStatus();
    }
    Connections {
        target: tlog
        onUpdated: {
            if (tlog.tlogId === author.id)
                author = tlog.author;
        }
    }
    MyFlickable {
        id: flickable
        anchors.fill: parent
        bottomMargin: 1.5 * mm
        contentWidth: parent.width
        contentHeight: column.height
//        interactive: profileView.x == 0
        Poppable {
            body: profileView
        }
        Column {
            id: column
            width: window.width
            spacing: 1.5 * mm
            visible: !tlog.loading && !tlog.errorString
            MyImage {
                id: bigAvatar
                width: window.width
                height: width
                url: Settings.maxImageWidth > 800 ? author.originalPic : author.largePic
                savable: true
                popBody: profileView
            }
            ThemedText {
                id: name
                width: window.width
                font.pointSize: window.fontBiggest
                horizontalAlignment: Text.AlignHCenter
                text: author.name
                height: text.length > 0 ? paintedHeight : 0
            }
            ThemedText {
                width: window.width
                horizontalAlignment: Text.AlignHCenter
                text: author.title
                height: text.length > 0 ? paintedHeight : 0
            }
            ThemedText {
                width: window.width
                horizontalAlignment: Text.AlignHCenter
                text: author.isFemale ? 'Девушка' : 'Парень'
                height: text.length > 0 ? paintedHeight : 0
                visible: !author.isFlow
            }
            ThemedText {
                width: window.width
                horizontalAlignment: Text.AlignHCenter
                text: author.lastSeenAt
                height: text.length > 0 ? paintedHeight : 0
                visible: !author.isFlow
            }
            ThemedText {
                width: window.width
                horizontalAlignment: Text.AlignHCenter
                text: (author.isPrivacy ? 'Закрытый ' : 'Открытый ')
                      + (author.isFlow ? 'поток' : 'тлог')
                height: text.length > 0 ? paintedHeight : 0
            }
            ThemedText {
                width: window.width
                horizontalAlignment: Text.AlignHCenter
                text: author.daysCount
                height: text.length > 0 ? paintedHeight : 0
            }
            ThemedText {
                width: window.width
                horizontalAlignment: Text.AlignHCenter
                text: {
                    switch (tlog.myRelationship) {
                    case Tlog.Friend:
                        'Вы подписаны';         break;
                    case Tlog.None:
                        'Вы не подписаны';      break;
                    case Tlog.Me:
                        'Это вы';               break;
                    case Tlog.Requested:
                        'Вы отправили запрос';  break;
                    case Tlog.Ignored:
                        'Тлог заблокирован';    break;
                    default:
                        '';
                    }
                }
                height: text.length > 0 ? paintedHeight : 0
                visible: tlog.myRelationship !== Tlog.Undefined
                         && Tasty.isAuthorized
            }
            ThemedText {
                width: window.width
                horizontalAlignment: Text.AlignHCenter
                text: {
                    switch (tlog.hisRelationship) {
                    case Tlog.Friend:
                        'Следит за вашим тлогом';    break;
                    case Tlog.None:
                        'Не следит за вашим тлогом'; break;
                    case Tlog.Me:
                        'Это вы';                    break;
                    case Tlog.Requested:
                        'Ожидает одобрения';         break;
                    case Tlog.Ignored:
                        'Вы заблокированы';          break;
                    default:
                        '';
                    }
                }
                height: text.length > 0 ? paintedHeight : 0
                visible: !author.isFlow
                         && tlog.hisRelationship !== Tlog.Undefined && tlog.hisRelationship !== Tlog.Me
                         && Tasty.isAuthorized
            }
            Row {
                width: window.width
                height: tlogVoteButton.height
                property int tlogMode: Trainer.typeOfTlog(tlog.tlogId)
                visible: readButton.enabled
                spacing: 1.5 * mm
                leftPadding: 1.5 * mm
                rightPadding: 1.5 * mm
                IconButton {
                    id: tlogVoteAgainstButton
                    width: (parent.width - 4.5 * mm) / 2
                    icon: (parent.tlogMode == 0 //Trainer.WaterMode
                          ? '../icons/drop-solid-' : '../icons/drop-outline-')
                          + '72.png'
                    enabled: parent.tlogMode == 0 || parent.tlogMode == 2 //Trainer.UndefinedMode
                    onClicked: {
                        parent.tlogMode = 0;
                        Trainer.trainTlog(tlog.tlogId, tlog.author.name, 0);
                    }
                }
                IconButton {
                    id: tlogVoteButton
                    width: tlogVoteAgainstButton.width
                    icon: (parent.tlogMode == 1 //Trainer.FireMode
                          ? '../icons/flame-solid-' : '../icons/flame-outline-')
                          + '72.png'
                    enabled: parent.tlogMode == 1 || parent.tlogMode == 2 //Trainer.UndefinedMode
                    onClicked: {
                        parent.tlogMode = 1;
                        Trainer.trainTlog(tlog.tlogId, tlog.author.name, 1);
                    }
                }
            }
            Grid {
                width: window.width
                columns: 3
                padding: 1.5 * mm
                spacing: 1.5 * mm
                readonly property int cellHeight: readButton.implicitHeight
                readonly property int cellWidth: (window.width - 6 * mm) / 3
                ThemedButton {
                    id: readButton
                    width: parent.cellWidth
                    height: parent.cellHeight
                    font.pointSize: window.fontSmallest
                    text: tlog.publicEntriesCount
                    onClicked: window.pushTlog(author.id)
                    enabled: (!author.isPrivacy
                              || tlog.myRelationship === Tlog.Friend
                              || tlog.myRelationship === Tlog.Me)
                             && tlog.hisRelationship !== Tlog.Ignored
                }
                ThemedButton {
                    width: parent.cellWidth
                    height: parent.cellHeight
                    font.pointSize: window.fontSmallest
                    text: tlog.tagsCount
                    onClicked: window.pushTags(tlog)
//                    visible: !author.isFlow
                    enabled: readButton.enabled
                }
                ThemedButton {
                    id: favButton
                    width: parent.cellWidth
                    height: parent.cellHeight
                    font.pointSize: window.fontSmallest
                    text: tlog.favoritesCount
                    onClicked: window.pushTlog(author.id, 0, '', FeedModel.FavoritesMode)
                    visible: !author.isFlow
                    enabled: readButton.enabled
                }
                ThemedButton {
                    id: followersButton
                    width: parent.cellWidth
                    height: parent.cellHeight
                    font.pointSize: window.fontSmallest
                    text: tlog.followersCount
                    onClicked: window.pushUsers(UsersModel.FollowersMode, author.id, tlog)
                }
                ThemedButton {
                    id: ignoredButton
                    width: parent.cellWidth
                    height: parent.cellHeight
                    font.pointSize: window.fontSmallest
                    text: tlog.ignoredCount
                    visible: !author.isFlow
                    onClicked: window.pushUsers(UsersModel.MyIgnoredMode, author.id, tlog)
                    enabled: tlog.myRelationship === Tlog.Me
                }
                ThemedButton {
                    width: parent.cellWidth
                    height: parent.cellHeight
                    font.pointSize: window.fontSmallest
                    text: tlog.followingsCount
                    visible: !author.isFlow
                    onClicked: window.pushUsers(UsersModel.FollowingsMode, author.id, tlog)
                }
            }
        }
    }
}
