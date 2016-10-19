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
import org.binque.taaasty 1.0

Item {
    id: menu
    anchors {
        top: parent.top
        bottom: parent.bottom
        left: parent.left
    }
    width: 40 * mm
    function changeMode(mode) {
        backAnimation.start();
        window.setFeedMode(mode);
    }
    onVisibleChanged: {
        window.hideFooter();
    }
    Flickable {
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick
        topMargin: 1.5 * mm
        bottomMargin: 1.5 * mm
        contentWidth: parent.width
        contentHeight: column.height
        Column {
            id: column
            width: parent.width
            spacing: 1.5 * mm
            MenuItem {
                text: 'Мой тлог'
                onTriggered: { changeMode(FeedModel.MyTlogMode) }
                visible: Tasty.isAuthorized
                highlighted: stack.currentItem.mode === FeedModel.MyTlogMode
            }
            MenuItem {
                text: 'Подписки' + (Tasty.unreadFriendsEntries > 0 
                        ? ' <b>+' + Tasty.unreadFriendsEntries + '</b>' : '')
                onTriggered: { changeMode(FeedModel.FriendsMode) }
                visible: Tasty.isAuthorized
                highlighted: stack.currentItem.mode === FeedModel.FriendsMode
            }
            MenuSeparator {
                visible: Tasty.isAuthorized
            }
            MenuItem {
                text: 'Прямой эфир'
                onTriggered: { changeMode(FeedModel.LiveMode) }
                highlighted: stack.currentItem.mode === FeedModel.LiveMode
                             || stack.currentItem.mode === FeedModel.FlowsMode
            }
            MenuItem {
                text: 'Лучшее'
                onTriggered: { changeMode(FeedModel.BestMode) }
                highlighted: stack.currentItem.bestMode === true
            }
            MenuItem {
                text: 'Анонимки'
                onTriggered: { changeMode(FeedModel.AnonymousMode) }
                highlighted: stack.currentItem.mode === FeedModel.AnonymousMode
            }
            MenuItem {
                text: 'Потоки'
                onTriggered: { 
                    backAnimation.start();
                    window.pushFlows();
                }
            }
            MenuItem {
                text: 'Тлог'
                onTriggered: { 
                    backAnimation.start();
                    window.showLineInput('tlog');
                }
                highlighted: stack.currentItem.mode === FeedModel.TlogMode
            }
            MenuSeparator {
                visible: Tasty.isAuthorized
            }
            MenuItem {
                text: 'Избранное'
                onTriggered: { changeMode(FeedModel.MyFavoritesMode) }
                visible: Tasty.isAuthorized
                highlighted: stack.currentItem.mode === FeedModel.MyFavoritesMode
            }
            MenuSeparator { }
            MenuItem {
                text: 'Войти'
                visible: !Tasty.isAuthorized
                onTriggered: {
                    backAnimation.start();
                    window.pushLoginDialog();
                }
            }
            MenuItem {
                text: 'Настройки'
                onTriggered: { 
                    backAnimation.start();
                    window.pushSettings();
                }
            }
        }
    }
}
