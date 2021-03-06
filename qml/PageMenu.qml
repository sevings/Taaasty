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

Q.Drawer {
    id: drawer
    edge: Qt.RightEdge
    height: window.height
    width: 40 * mm
    property Item page: Item { }
    property bool openable: true
    readonly property bool autoclose: !openable || !(page.hasMenu || window.chatsShows || window.notifsShows)
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
        height: Math.min(parent.height, contentHeight)
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
                         && !window.notifsShows
                         && page.isFullEntryView === true
                         && page.entry
                         && page.entry.author
                         && page.entry.author.id > 0
                         && page.tlog.tlogId !== page.entry.author.id
                         && page.entry.type != TlogEntry.AnonymousEntry
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
                         && !window.notifsShows
                         && tlog && tlog.tlogId > 0
                         && (!page.isFullEntryView
                             || page.entry.type != TlogEntry.AnonymousEntry)
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
                visible: !window.chatsShows
                         && !window.notifsShows
                         && page.isFeedView === true
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
                visible: !window.chatsShows
                         && !window.notifsShows
                         && page.isFeedView === true
                         && (   page.mode === FeedModel.MyTlogMode
                             || page.mode === FeedModel.MyPrivateMode)
            }
            MenuSeparator {
                visible: !window.chatsShows
                         && !window.notifsShows
                         && page.isFeedView === true
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
                         && !window.notifsShows
                         && page.isFeedView === true
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
                         && !window.notifsShows
                         && page.isFeedView === true
                         && (   page.mode === FeedModel.LiveMode
                             || page.mode === FeedModel.FlowsMode)
            }
            MenuSeparator {
                visible: !window.chatsShows
                         && !window.notifsShows
                         && page.isFeedView === true
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
                         && !window.notifsShows
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
                         && !window.notifsShows
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
                         && !window.notifsShows
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
                         && !window.notifsShows
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
                         && !window.notifsShows
                         && page.bestMode === true
            }
            MenuSeparator {
                visible: !window.chatsShows
                         && !window.notifsShows
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
                         && !window.notifsShows
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
            MenuSeparator {
                visible: window.chatsShows
            }
            MenuItem {
                text: 'Обновить'
                onTriggered: {
                    ChatsModel.loadLast();
                    Tasty.reconnectToPusher();
                    drawer.close();
                }
                enabled: visible && !ChatsModel.checking
                visible: window.chatsShows
            }

            // NOTIFICATIONS SECTION
            MenuItem {
                text: 'Уведомления'
                onTriggered: {
                    window.friendActivity = false;
                    drawer.close();
                }
                highlighted: !window.friendActivity
                visible: window.notifsShows
            }
            MenuItem {
                text: 'Активность'
                onTriggered: {
                    window.friendActivity = true;
                    drawer.close();
                }
                highlighted: window.friendActivity
                visible: window.notifsShows
            }
            MenuSeparator {
                visible: window.notifsShows
            }
            MenuItem {
                text: 'Обновить'
                onTriggered: {
                    (window.friendActivity ? FriendActivityModel : NotifsModel).check();
                    drawer.close();
                }
                enabled: visible && !(window.friendActivity ? FriendActivityModel : NotifsModel).checking
                visible: window.notifsShows
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
                         && !window.notifsShows
                         && page.isMessagesView === true
                         && page.chat.entry //&& page.chat.isInvolved
            }
            MenuSeparator {
                visible: chatTopic.visible
            }
            MenuItem {
                id: attachImage
                text: 'Прикрепить'
                onTriggered: {
                    window.pushUploadView(page.chat.images);
                    drawer.close();
                }
                visible: !window.chatsShows
                         && !window.notifsShows
                         && page.isMessagesView === true
                         && !page.chat.sending
            }
            MenuSeparator {
                visible: attachImage.visible
            }
            MenuItem {
                text: 'Прочитать все'
                onTriggered: {
                    page.chat.readAll();
                    drawer.close();
                }
                visible: !window.chatsShows
                         && !window.notifsShows
                         && page.isMessagesView === true
                         && page.chat.unreadCount > 0
            }
            MenuItem {
                text: 'Перезагрузить'
                onTriggered: {
                    page.chat.messages.reset();
                    page.reload();
                    drawer.close();
                }
                visible: !window.chatsShows
                         && !window.notifsShows
                         && page.isMessagesView === true
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
                         && !window.notifsShows
                         && page.isFullEntryView === true
                         && page.entry.isFavoritable
            }
            MenuItem {
                text: 'Пожаловаться'
                onTriggered: {
                    window.askUser('Отправить жалобу в службу поддержки?', function() {
                      page.entry.report();
                    });
                    drawer.close();
                }
                visible: !window.chatsShows
                         && !window.notifsShows
                         && page.isFullEntryView === true
                         && page.entry.isReportable
            }
            MenuItem {
                text: 'Перезагрузить'
                onTriggered: {
                    page.entry.reload();
                    drawer.close();
                }
                visible: !window.chatsShows
                         && !window.notifsShows
                         && page.isFullEntryView === true
                         && !page.entry.loading
            }
            MenuItem {
                text: 'Открыть в браузере'
                onTriggered: {
                    var url = page.entry.url;
                    Qt.openUrlExternally(url);
                    drawer.close();
                }
                visible: !window.chatsShows
                         && !window.notifsShows
                         && page.isFullEntryView === true
                         && page.entry.url
            }
            MenuSeparator {
                visible: !window.chatsShows
                         && !window.notifsShows
                         && page.isFullEntryView === true
                         && !page.entry.loading
                         && Tasty.isAuthorized
                         && (goChat.visible || leftChat.visible)
            }
            MenuItem {
                id: goChat
                text: 'Перейти к беседе'
                onTriggered: {
                    window.pushMessages(page.entry.chat);
                    drawer.close();
                }
                visible: !window.chatsShows
                         && !window.notifsShows
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
                    window.askUser(leftChat.text + '?', function() {
                        if (page.isFullEntryView)
                            page.entry.chat.remove();
                        else
                            page.chat.remove();
                    });
                    drawer.close();
                }
                visible: !window.chatsShows
                         && !window.notifsShows
                         && ((page.isFullEntryView === true
                             && page.entry.chat
                             && page.entry.chat.isInvolved)
                         || (page.isMessagesView === true
                             && page.chat.isInvolved))
                         && Tasty.isAuthorized
            }
            MenuSeparator {
                visible: !window.chatsShows
                         && !window.notifsShows
                         && page.isFullEntryView === true
                         && !page.entry.loading
                         && (page.entry.isDeletable || page.isUnrepostable)
            }
            MenuItem {
                text: 'Редактировать'
                onTriggered: {
                    window.pushEntryEditor(page.entry);
                    drawer.close();
                }
                visible: !window.chatsShows
                         && !window.notifsShows
                         && page.isFullEntryView === true
                         && page.entry.isEditable
                         && (page.entry.type === TlogEntry.TextEntry
                             || page.entry.type === TlogEntry.QuoteEntry
                             || page.entry.type === TlogEntry.AnonymousEntry
                             || page.entry.type === TlogEntry.VideoEntry)
            }
            MenuItem {
                text: 'Удалить запись'
                onTriggered: {
                    window.askUser('Удалить пост?', function() {
                      page.entry.deleteEntry();
                    });
                    drawer.close();
                }
                visible: !window.chatsShows
                         && !window.notifsShows
                         && page.isFullEntryView === true
                         && page.entry.isDeletable
            }
            MenuItem {
                text: 'Удалить репост'
                onTriggered: {
                    window.askUser('Удалить репост?', function() {
                        if (page.feedModel)
                            page.feedModel.reposter.unrepost(page.entry.entryId);
                    });
                    drawer.close();
                }
                visible: !window.chatsShows
                         && !window.notifsShows
                         && page.isFullEntryView === true
                         && page.isUnrepostable
                         && Tasty.isAuthorized
            }

            // FLOWS SECTION
            MenuItem {
                text: 'Создать'
                onTriggered: {
                    window.pushFlowEditor();
                    drawer.close();
                }
                visible: !window.chatsShows
                         && !window.notifsShows
                         && page.isFlowsView === true
            }
            MenuSeparator {
                visible: !window.chatsShows
                         && !window.notifsShows
                         && page.isFlowsView === true
            }
            MenuItem {
                text: 'Популярные'
                onTriggered: {
                    page.mode = 0; //= FlowsModel.PopularMode
                    drawer.close();
                }
                highlighted: page.mode === FlowsModel.PopularMode
                visible: !window.chatsShows
                         && !window.notifsShows
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
                         && !window.notifsShows
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
                         && !window.notifsShows
                         && page.isFlowsView === true
                         && Tasty.isAuthorized
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
                         && !window.notifsShows
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
                    window.askUser('Отписать от вас ' + page.tlog.author.name + '?', function() {
                      page.tlog.unsubscribeHim();
                    });
                    drawer.close();
                }
                visible: !window.chatsShows
                         && !window.notifsShows
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
                         && !window.notifsShows
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
                         && !window.notifsShows
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
                         && !window.notifsShows
                         && page.isProfileView === true
                         && page.tlog.hisRelationship === Tlog.Requested
                enabled: visible && !page.tlog.changingRelation
            }
            MenuSeparator {
                visible: tlogMessage.visible
                         && page.tlog.myRelationship !== Tlog.Me
            }
            MenuItem {
                id: tlogMessage
                text: 'Написать сообщение'
                onTriggered: {
                    window.pushMessages(page.tlog.chat);
                    drawer.close();
                }
                visible: !window.chatsShows
                         && !window.notifsShows
                         && page.isProfileView === true
                         && !page.tlog.errorString
                         && !page.author.isFlow
                         && page.tlog.hisRelationship !== Tlog.Ignored
                         && Tasty.isAuthorized
                enabled: visible
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
                         && !window.notifsShows
                         && page.isProfileView === true
                         && !page.tlog.errorString
                         && (!page.author.isPrivacy
                             || page.tlog.myRelationship === Tlog.Friend
                             || page.tlog.myRelationship === Tlog.Me)
                         && page.tlog.hisRelationship !== Tlog.Ignored
            }
            MenuSeparator {
                visible: editFlow.visible
                         && Tasty.isAuthorized
            }
            MenuItem {
                id: editFlow
                text: 'Редактировать'
                onTriggered: {
                    window.pushFlowEditor(page.tlog.flow);
                    drawer.close();
                }
                visible: !window.chatsShows
                         && !window.notifsShows
                         && page.isProfileView === true
                         && !page.tlog.errorString
                         && page.tlog.flow
                         && page.tlog.flow.isEditable
            }

            // CALENDAR SECTION
            MenuItem {
                text: 'Сначала новые'
                onTriggered: {
                    page.sortOrder = CalendarModel.NewestFirst
                    drawer.close();
                }
                highlighted: page.sortOrder == CalendarModel.NewestFirst
                visible: !window.chatsShows
                         && !window.notifsShows
                         && page.isCalendarView === true
            }
            MenuItem {
                text: 'Сначала лучшие'
                onTriggered: {
                    page.sortOrder = CalendarModel.BestFirst
                    drawer.close();
                }
                highlighted: page.sortOrder == CalendarModel.BestFirst
                visible: !window.chatsShows
                         && !window.notifsShows
                         && page.isCalendarView === true
            }

            // ENTRY EDITOR SECTION
            MenuItem {
                text: 'Текст'
                onTriggered: {
                    page.entryType = TlogEntry.TextEntry;
                    drawer.close();
                }
                highlighted: page.entryType === TlogEntry.TextEntry
                visible: !window.chatsShows
                         && !window.notifsShows
                         && page.isEntryEditor === true
            }
            MenuItem {
                text: 'Цитата'
                onTriggered: {
                    page.entryType = TlogEntry.QuoteEntry;
                    drawer.close();
                }
                highlighted: page.entryType === TlogEntry.QuoteEntry
                visible: !window.chatsShows
                         && !window.notifsShows
                         && page.isEntryEditor === true
            }
            MenuItem {
                text: 'Изображение'
                onTriggered: {
                    page.entryType = TlogEntry.ImageEntry;
                    drawer.close();
                }
                highlighted: page.entryType === TlogEntry.ImageEntry
                visible: !window.chatsShows
                         && !window.notifsShows
                         && page.isEntryEditor === true
            }
            MenuItem {
                text: 'Ссылка'
                onTriggered: {
                    page.entryType = TlogEntry.VideoEntry;
                    drawer.close();
                }
                highlighted: page.entryType === TlogEntry.VideoEntry
                visible: !window.chatsShows
                         && !window.notifsShows
                         && page.isEntryEditor === true
            }
            MenuItem {
                text: 'Анонимка'
                onTriggered: {
                    page.entryType = TlogEntry.AnonymousEntry;
                    drawer.close();
                }
                highlighted: page.entryType === TlogEntry.AnonymousEntry
                visible: !window.chatsShows
                         && !window.notifsShows
                         && page.isEntryEditor === true
            }
        }
    }
}
