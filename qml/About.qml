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
    MyFlickable {
        id: flick
        anchors.fill: parent
        contentHeight: item.height
        Item {
            id: item
            width: window.width
            height: Math.max(aboutText.contentHeight, window.height)
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
                      + '<p>Программа доступна на <a href="https://play.google.com/store/apps/details?id=org.binque.taaasty">Google Play</a>. Исходный код под лицензией <a href="http://www.gnu.org/licenses/gpl.html">GNU&nbsp;GPLv3</a> (и частично — BSD) доступен на GitHub. </p>'
                      + '<p>Программа распространяется <b>как&nbsp;есть</b> безо всяких гарантий и условий, явных и подразумеваемых, в надежде, что она кому-то окажется полезной.</p>'
                      + '<p>Со всеми вопросами, сообщениями о проблемах, пожеланиями, а также с благодарностями вы можете <a href="http://taaasty.com/~1409820">обращаться к автору.</a></p>'
                      + '<p>Иконки сделаны <a href="http://www.flaticon.com/authors/catalin-fertu">Catalin Fertu</a> и <a href="http://www.flaticon.com/authors/freepik">Freepik</a> с www.flaticon.com под лицензией CC&nbsp;BY&nbsp;3.0.</p>'
            }
        }
    }
}
