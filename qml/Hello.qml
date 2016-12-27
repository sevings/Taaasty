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

Pane {
    id: back
    innerFlick: flick
    function openLink(link) {
        if (link === 'help')
            window.pushHelp();
        else
            window.openLink(link);
    }
    Poppable {
        body: back
    }
    MyFlickable {
        id: flick
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            right: parent.right
        }
        height: Math.min(contentHeight, parent.height)
        topMargin: 1.5 * mm
        bottomMargin: 1.5 * mm
        flickableDirection: Flickable.VerticalFlick
        contentHeight: helloText.implicitHeight
        contentWidth: parent.width
        Poppable {
            body: back
        }
        ThemedText {
            id: helloText
            anchors {
                left: parent.left
                right: parent.right
            }
            onLinkActivated: back.openLink(link)
            text: '<h3>Привет, <a href="http://taaasty.com/~' + Settings.iserId + '">' + Settings.login + '</a>!</h3>'
                + '<p>Здорово, что ты пользуешься альтернативным клиентом Тейсти.</p>'
                + '<p>Если у тебя есть вопросы или пожелания по работе приложения, <a href="help">можешь посмотреть здесь</a>.</p>'
                + '<p>Чтобы быть в курсе последних изменений, <a href="http://taaasty.com/~1431729">подписывайся на поток</a>.</p>'
                + '<p>И если тебе нравится приложение, <a href="https://play.google.com/store/apps/details?id=org.binque.taaasty">не забудь оставить отзыв</a>.</p>'
                + '<p>Приятного использования!</p>'
        }
    }
}
