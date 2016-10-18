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
//        interactive: back.x == 0
        Poppable {
            body: back
        }
        Column {
            id: column
            width: window.width
            spacing: 1.5 * mm
            padding: 1.5 * mm
            Component.onCompleted: {
                imagesBox.checked = Settings.autoloadImages;
                shortBox.checked  = Settings.hideShortPosts;
                nbcBox.checked    = Settings.hideNegativeRated;
                darkBox.checked   = Settings.darkTheme;
                notiBox.checked   = Settings.systemNotifications
            }
            ThemedText {
                text: 'Настройки'
                width: parent.width - parent.padding * 2
                font.pointSize: window.fontBigger
                horizontalAlignment: Text.AlignHCenter
            }
            ThemedCheckBox {
                id: shortBox
                text: 'Скрывать короткие посты'
                onCheckedChanged: { Settings.hideShortPosts = checked; }
            }
            ThemedCheckBox {
                id: nbcBox
                text: 'Включить НБК'
                onCheckedChanged: { Settings.hideNegativeRated = checked; }
            }
            ThemedCheckBox {
                id: darkBox
                text: 'Темная тема'
                onCheckedChanged: { Settings.darkTheme = checked; }
            }
            ThemedCheckBox {
                id: notiBox
                text: 'Системные уведомления'
                onCheckedChanged: { Settings.systemNotifications = checked; }
            }
            ThemedCheckBox {
                id: imagesBox
                text: 'Загружать изображения'
                onCheckedChanged: { Settings.autoloadImages = checked; }
            }
            ThemedButton {
                anchors.horizontalCenter: parent.horizontalCenter
                implicitWidth: 40 * mm
                highlighted: true
                text: 'Очистить кэш'
                onClicked: {
                    enabled = false;
                    Cache.clearUnusedImages();
                }
            }
            MenuSeparator {
                width: implicitWidth - parent.padding * 2
            }
            ThemedText {
                width: parent.width - parent.padding * 2
                horizontalAlignment: Text.AlignHCenter
                text: Tasty.isAuthorized ? 'Вы вошли как <i>' + Settings.login + '</i>' : 'Вы не вошли'
            }
            ThemedButton {
                id: prevLoginButton
                anchors.horizontalCenter: parent.horizontalCenter
                implicitWidth: 40 * mm
                highlighted: true
                text: Settings.prevLogin
                visible: Settings.prevLogin.length > 0
                onClicked: {
                    enabled = false;
                    Tasty.swapProfiles();
                }
                Connections {
                    target: Tasty
                    onAuthorized: {
                        prevLoginButton.enabled = true;
                    }
                }
            }
            ThemedButton {
                anchors.horizontalCenter: parent.horizontalCenter
                implicitWidth: 40 * mm
                highlighted: true
                text: Tasty.isAuthorized ? 'Сменить тлог' : 'Войти'
                onClicked: window.pushLoginDialog()
            }
        }
    }
}
