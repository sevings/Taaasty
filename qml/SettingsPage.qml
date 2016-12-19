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
        Poppable {
            body: back
        }
        Column {
            id: column
            width: window.width
            spacing: 1.5 * mm
            padding: 1.5 * mm
            Component.onCompleted: {
                shortBox.checked      = Settings.hideShortPosts;
                nbcBox.checked        = Settings.hideNegativeRated;
                darkBox.checked       = Settings.darkTheme;
                notiBox.checked       = Settings.systemNotifications;
                textBox.checked       = Settings.predictiveText;
                imageSlider.setSize(Settings.maxLoadImageSize);
                imageWifiBox.checked  = Settings.loadImagesOverWifi;
            }
            ThemedText {
                text: 'Основные'
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
                id: textBox
                text: 'Предиктивный ввод'
                onCheckedChanged: { Settings.predictiveText = checked; }
            }
            MenuSeparator {
                width: implicitWidth - parent.padding * 2
                height: 3 * mm
            }
            ThemedText {
                text: 'Изображения'
                width: parent.width - parent.padding * 2
                font.pointSize: window.fontBigger
                horizontalAlignment: Text.AlignHCenter
            }
            ThemedText {
                id: imageText
                width: parent.width - parent.padding * 2
                text: imageSlider.position < 1 
                    ? imageSlider.size > 0
                    ? 'Загружать до ' + imageSlider.size + ' КБ'
                    : 'Не загружать'
                    : 'Загружать всегда'
            }
            Q.Slider {
                id: imageSlider
                width: parent.width - parent.padding * 2
                snapMode: Q.Slider.SnapAlways
                stepSize: 0.02
                onValueChanged: { Settings.maxLoadImageSize = value < 1 ? size : -1; }
                readonly property int size: Math.pow(position, 2) * 10000
                function setSize(v) {
                    if (v < 0)
                        value = 1;
                    else
                        value = Math.sqrt(v / 10000);
                }
            }
            ThemedCheckBox {
                id: imageWifiBox
                text: 'Загружать все через Wi-Fi'
                visible: imageSlider.value < 1
                onCheckedChanged: { Settings.loadImagesOverWifi = checked; }
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
                height: 3 * mm
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
                    onAuthorizedChanged: {
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
            ThemedButton {
                anchors.horizontalCenter: parent.horizontalCenter
                implicitWidth: 40 * mm
                highlighted: true
                visible: Tasty.isAuthorized
                text: 'Выйти'
                onClicked: Tasty.logout()
            }
            MenuSeparator {
                width: implicitWidth - parent.padding * 2
                height: 3 * mm
            }
            ThemedButton {
                anchors.horizontalCenter: parent.horizontalCenter
                implicitWidth: 40 * mm
                highlighted: true
                text: 'Помощь'
                onClicked: { window.pushHelp(); }
            }
            ThemedButton {
                anchors.horizontalCenter: parent.horizontalCenter
                implicitWidth: 40 * mm
                highlighted: true
                text: 'О клиенте'
                onClicked: { window.pushAbout(); }
            }
        }
    }
}
