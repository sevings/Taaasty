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
import TextReader 1.0

Pane {
    id: back
    innerFlick: flick
    MyFlickable {
        id: flick
        anchors.fill: parent
        contentHeight: item.height
//        interactive: back.x == 0
        Item {
            id: item
            width: window.width
            height: aboutText.contentHeight > window.height
                    ? aboutText.contentHeight : window.height
            Poppable {
                body: back
            }
            ThemedText {
                id: aboutText
                anchors {
                    left: parent.left
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }
                onLinkActivated: window.openLink(link)
                text: Qt.application.name + ' ' + Qt.application.version + ' (' + builtAt + ').<br>'
                      + reader.read()
            }
            TextReader {
                id: reader
                source: ':/other/about.html'
            }
        }
    }
}
