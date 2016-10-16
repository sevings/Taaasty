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

Q.Drawer {
    id: drawer
    edge: Qt.RightEdge
    height: window.height
    width: 40 * mm
    property Item page: Item { }
    property bool openable: true
    readonly property bool autoclose: !openable || column.height < 5 * mm
    onOpened: {
        if (autoclose)
            drawer.close();
    }
    Flickable {
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            right: parent.right
        }
        height: contentHeight > parent.height ? parent.height : contentHeight
        flickableDirection: Flickable.VerticalFlick
        topMargin: 1.5 * mm
        bottomMargin: 1.5 * mm
        contentWidth: parent.width
        contentHeight: column.height
        Column {
            id: column
            width: parent.width
            spacing: 1.5 * mm

            // PROFILES SECTION
            Item {
                visible: !window.chatsShows
                         && page.isFullEntryView === true
                         && page.entry
                         && page.entry.author
                         && page.entry.author.id > 0
                         && page.tlog.tlogId !== page.entry.author.id
                         && page.entry.author.id !== window.anonymousId
                height: 6 * mm
                width: parent.width
                SmallAvatar {
                    id: entryAuthorAvatar
                    anchors.leftMargin: 1.5 * mm
                    width: 6 * mm
                    height: 6 * mm
                    user: parent.visible ? page.entry.author : null
                }
                MenuItem {
                    leftPadding: entryAuthorAvatar.width + entryAuthorAvatar.anchors.leftMargin * 2
                    height: parent.height
                    text: parent.visible ? page.entry.author.name : ''
                    onTriggered: {
                        window.pushProfileById(page.entry.author.id);
                        drawer.close();
                    }
                }
            }
            Item {
                id: pageProfile
                visible: !window.chatsShows
                         && tlog && tlog.tlogId > 0
                         && tlog.tlogId !== window.anonymousId
                height: 6 * mm
                width: parent.width
                property Tlog tlog: page.isFullEntryView
                                    || (page.isFeedView && page.mode === FeedModel.TlogMode)
                                    || (page.isMessagesView && page.chat.type === Chat.PrivateConversation)
                                    ? page.tlog : null
                SmallAvatar {
                    id: tlogAvatar
                    anchors.leftMargin: 1.5 * mm
                    width: 6 * mm
                    height: 6 * mm
                    user: pageProfile.tlog ? pageProfile.tlog.author : null
                }
                MenuItem {
                    leftPadding: tlogAvatar.width + tlogAvatar.anchors.leftMargin * 2
                    height: parent.height
                    text: pageProfile.tlog ? pageProfile.tlog.author.name : ''
                    onTriggered: {
                        window.pushProfile(pageProfile.tlog);
                        drawer.close();
                    }
                }
            }
            MenuSeparator {
                visible: pageProfile.visible
            }

            // MY TLOG SECTION
            MenuItem {
                text: 'Все записи'
                onTriggered: {
                    window.setFeedMode(FeedModel.MyTlogMode);
                    drawer.close();
                }
                highlighted: page.mode === FeedModel.MyTlogMode
                visible: !window.chatsShows && page.isFeedView === true
                         && (   page.mode === FeedModel.MyTlogMode
                             || page.mode === FeedModel.MyPrivateMode)
            }
            MenuItem {
                text: 'Скрытые'
                onTriggered: {
                    window.setFeedMode(FeedModel.MyPrivateMode);
                    drawer.close();
                }
                highlighted: page.mode === FeedModel.MyPrivateMode
                visible: !window.chatsShows && page.isFeedView === true
                         && (   page.mode === FeedModel.MyTlogMode
                             || page.mode === FeedModel.MyPrivateMode)
            }
            MenuSeparator {
                visible: !window.chatsShows && page.isFeedView === true
                         && (   page.mode === FeedModel.MyTlogMode
                             || page.mode === FeedModel.MyPrivateMode)
            }

            // LIVE SECTION
            MenuItem {
                text: 'Все посты'
                onTriggered: {
                    window.setFeedMode(FeedModel.LiveMode);
                    drawer.close();
                }
                highlighted: page.mode === FeedModel.LiveMode
                visible: !window.chatsShows
                         && (   page.mode === FeedModel.LiveMode
                             || page.mode === FeedModel.FlowsMode)
            }
            MenuItem {
                text: 'Потоки'
                onTriggered: {
                    window.setFeedMode(FeedModel.FlowsMode);
                    drawer.close();
                }
                highlighted: page.mode === FeedModel.FlowsMode
                visible: !window.chatsShows
                         && (   page.mode === FeedModel.LiveMode
                             || page.mode === FeedModel.FlowsMode)
            }
            MenuSeparator {
                visible: !window.chatsShows
                         && (   page.mode === FeedModel.LiveMode
                             || page.mode === FeedModel.FlowsMode)
            }


            // BEST SECTION
            MenuItem {
                text: 'Лучшее'
                onTriggered: {
                    window.setFeedMode(FeedModel.BestMode);
                    drawer.close();
                }
                highlighted: page.mode === FeedModel.BestMode
                visible: !window.chatsShows
                         && page.bestMode === true
            }
            MenuItem {
                text: 'Отличное'
                onTriggered: {
                    window.setFeedMode(FeedModel.ExcellentMode);
                    drawer.close();
                }
                highlighted: page.mode === FeedModel.ExcellentMode
                visible: !window.chatsShows
                         && page.bestMode === true
            }
            MenuItem {
                text: 'Хорошее'
                onTriggered: {
                    window.setFeedMode(FeedModel.WellMode);
                    drawer.close();
                }
                highlighted: page.mode === FeedModel.WellMode
                visible: !window.chatsShows
                         && page.bestMode === true
            }
            MenuItem {
                text: 'Неплохое'
                onTriggered: {
                    window.setFeedMode(FeedModel.GoodMode);
                    drawer.close();
                }
                highlighted: page.mode === FeedModel.GoodMode
                visible: !window.chatsShows
                         && page.bestMode === true
            }
            MenuItem {
                text: 'По рейтингу'
                onTriggered: {
                    window.setFeedMode(FeedModel.BetterThanMode);
                    drawer.close();
                }
                highlighted: page.mode === FeedModel.BetterThanMode
                visible: !window.chatsShows
                         && page.bestMode === true
            }
            MenuSeparator {
                visible: !window.chatsShows
                         && page.bestMode === true
            }

            // FEED SECTION
            MenuItem {
                text: 'Поиск'
                onTriggered: {
                    window.showLineInput('query');
                    drawer.close();
                }
                highlighted: visible && page.query.length > 0
                visible: !window.chatsShows
                         && page.isFeedView === true
            }

            // CHATS SECTION
            MenuItem {
                text: 'Все беседы'
                onTriggered: {
                    ChatsModel.mode = 0//ChatsModel.AllChatsMode
                    drawer.close();
                }
                highlighted: ChatsModel.mode == 0 //= ChatsModel.AllChatsMode
                visible: window.chatsShows
            }
            MenuItem {
                text: 'Личные'
                onTriggered: {
                    ChatsModel.mode = 1//ChatsModel.PrivateChatsMode
                    drawer.close();
                }
                highlighted: ChatsModel.mode == 1 //= ChatsModel.PrivateChatsMode
                visible: window.chatsShows
            }
            MenuItem {
                text: 'Комментарии'
                onTriggered: {
                    ChatsModel.mode = 2//ChatsModel.EntryChatsMode
                    drawer.close();
                }
                highlighted: ChatsModel.mode == 2 //= ChatsModel.EntryChatsMode
                visible: window.chatsShows
            }

            // MESSAGES SECTION
            MenuItem {
                id: chatTopic
                text: visible ? page.chat.topic : ''
                onTriggered: {
                    window.pushFullEntry(page.chat.entry)
                    drawer.close();
                }
                visible: !window.chatsShows
                         && page.isMessagesView === true
                         && page.chat.entry //&& page.chat.isInvolved
            }
            MenuSeparator {
                visible: chatTopic.visible && leftChat.visible
            }

            // ENTRY SECTION
            MenuItem {
                text: page.entry && page.entry.isFavorited
                      ? 'Удалить из избранного' : 'Добавить в избранное'
                onTriggered: {
                    page.entry.favorite();
                    drawer.close();
                }
                visible: !window.chatsShows
                         && page.isFullEntryView === true
                         && page.entry.isFavoritable
            }
            MenuItem {
                text: 'Открыть в браузере'
                onTriggered: {
                    var url = page.entry.url;
                    Qt.openUrlExternally(url);
                    drawer.close();
                }
                visible: !window.chatsShows
                         && page.isFullEntryView === true
                         && page.entry.url.length > 0
            }
            MenuItem {
                text: 'Перейти к беседе'
                onTriggered: {
                    window.pushMessages(page.entry.chat);
                    drawer.close();
                }
                visible: !window.chatsShows
                         && page.isFullEntryView === true
                         && page.entry.chat
                         && page.entry.chat.canTalk
                         && Tasty.isAuthorized
            }
            MenuItem {
                id: leftChat
                text: ((page.isFullEntryView === true
                        && page.entry.chat
                        && page.entry.chat.type === Chat.PrivateConversation)
                       || (page.isMessagesView === true
                           && page.chat.type === Chat.PrivateConversation))
                      ? 'Удалить переписку' : 'Покинуть беседу'
                onTriggered: {
                    if (page.isFullEntryView)
                        page.entry.chat.remove();
                    else
                        page.chat.remove();

                    drawer.close();
                }
                visible: !window.chatsShows
                         && (page.isFullEntryView === true
                             && page.entry.chat
                             && page.entry.chat.isInvolved)
                         || (page.isMessagesView === true
                             && page.chat.isInvolved)
                         && Tasty.isAuthorized
            }
            MenuSeparator {
                visible: !window.chatsShows
                         && page.isFullEntryView === true
                         && Tasty.isAuthorized
            }
            MenuItem {
                text: 'Перезагрузить'
                onTriggered: {
                    page.entry.reload();
                    drawer.close();
                }
                visible: !window.chatsShows
                         && page.isFullEntryView === true
            }

            // FLOWS SECTION
            MenuItem {
                text: 'Популярные'
                onTriggered: {
                    page.mode = 0; //= FlowsModel.PopularMode
                    drawer.close();
                }
                highlighted: page.mode === FlowsModel.PopularMode
                visible: !window.chatsShows
                         && page.isFlowsView === true
            }
            MenuItem {
                text: 'Новые'
                onTriggered: {
                    page.mode = 1; //= FlowsModel.NewestMode
                    drawer.close();
                }
                highlighted: page.mode === FlowsModel.NewestMode
                visible: !window.chatsShows
                         && page.isFlowsView === true
            }
            MenuItem {
                text: 'Мои'
                onTriggered: {
                    page.mode = 2; //= FlowsModel.MyMode
                    drawer.close();
                }
                highlighted: page.mode === FlowsModel.MyMode
                visible: !window.chatsShows
                         && page.isFlowsView === true
            }

            // PROFILE SECTION
            MenuItem {
                text: visible ?
                          (page.tlog.myRelationship === Tlog.None
                           ? (page.tlog.author.isPrivacy ? 'Отправить запрос' : 'Подписаться')
                           : (page.tlog.myRelationship === Tlog.Requested ? 'Отменить запрос' : 'Отписаться'))
                        : ''
                onTriggered: {
                    if (page.tlog.myRelationship === Tlog.Friend
                            || page.tlog.myRelationship === Tlog.Requested)
                        page.tlog.unfollow();
                    else
                        page.tlog.follow();

                    drawer.close();
                }
                visible: !window.chatsShows
                         && page.isProfileView === true
                         && (page.tlog.myRelationship === Tlog.Friend
                             || page.tlog.myRelationship === Tlog.Requested
                             || page.tlog.myRelationship === Tlog.None)
                         && page.tlog.hisRelationship !== Tlog.Ignored
                enabled: visible && !page.tlog.changingRelation
            }
            MenuItem {
                text: 'Отписать'
                onTriggered: {
                    page.tlog.unsubscribeHim();
                    drawer.close();
                }
                visible: !window.chatsShows
                         && page.isProfileView === true
                         && (page.tlog.hisRelationship === Tlog.Friend)
                enabled: visible && !page.tlog.changingRelation
            }
            MenuItem {
                text: visible && page.tlog.myRelationship === Tlog.Ignored ? 'Разблокировать' : 'Заблокировать'
                onTriggered: {
                    if (page.tlog.myRelationship === Tlog.Ignored)
                        page.tlog.cancelIgnoring();
                    else
                        page.tlog.ignore();

                    drawer.close();
                }
                visible: !window.chatsShows
                         && page.isProfileView === true
                         && (page.tlog.myRelationship === Tlog.Ignored
                             || page.tlog.myRelationship === Tlog.None)
                enabled: visible && !page.tlog.changingRelation
            }
            MenuItem {
                text: 'Принять запрос'
                onTriggered: {
                    page.tlog.approveFriendRequest();
                    drawer.close();
                }
                visible: !window.chatsShows
                         && page.isProfileView === true
                         && page.tlog.hisRelationship === Tlog.Requested
                enabled: visible && !page.tlog.changingRelation
            }
            MenuItem {
                text: 'Отклонить запрос'
                onTriggered: {
                    page.tlog.disapproveFriendRequest();
                    drawer.close();
                }
                visible: !window.chatsShows
                         && page.isProfileView === true
                         && page.tlog.hisRelationship === Tlog.Requested
                enabled: visible && !page.tlog.changingRelation
            }
            MenuSeparator {
                visible: tlogCalendar.visible
                         && Tasty.isAuthorized
            }
            MenuItem {
                id: tlogCalendar
                text: 'Обзор постов'
                onTriggered: {
                    window.pushCalendar(page.tlog);
                    drawer.close();
                }
                visible: !window.chatsShows
                         && page.isProfileView === true
                         && (!page.author.isPrivacy
                             || page.tlog.myRelationship === Tlog.Friend
                             || page.tlog.myRelationship === Tlog.Me)
                         && page.tlog.hisRelationship !== Tlog.Ignored
            }
        }
    }
}
