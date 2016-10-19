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
    Poppable {
        body: back
    }
    MyFlickable {
        id: flick
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: contentHeight > parent.height ? parent.height : contentHeight
        contentWidth: parent.width
        contentHeight: column.height
        Poppable {
            body: back
        }
        Column {
            id: column
            width: window.width
            spacing: 1.5 * mm
            padding: 1.5 * mm
            ThemedText {
                text: 'Помощь'
                width: parent.width - parent.padding * 2
                font.pointSize: window.fontBigger
                horizontalAlignment: Text.AlignHCenter
            }
            HelpPageItem {
                width: parent.width - parent.padding * 2
                text: 'Здесь можно минусовать?\nЧто такое НБК?'
                entryId: 21449890
            }
            HelpPageItem {
                width: parent.width - parent.padding * 2
                popBody: back
                text: 'Что за серая полоса под содержимым поста?'
                entryId: 21449891
            }
            HelpPageItem {
                width: parent.width - parent.padding * 2
                popBody: back
                text: 'Как работает скрытие коротких постов?'
                entryId: 21449894
            }
            HelpPageItem {
                width: parent.width - parent.padding * 2
                popBody: back
                text: 'Задать другой вопрос'
                entryId: 21449900
            }
        }
    }
}
