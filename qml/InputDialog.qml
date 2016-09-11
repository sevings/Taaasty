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

Popup {
    id: back
    height: 4 * mm + label.height + input.height + acceptButton.height
    property string mode: 'tlog'
    property alias text: input.text
    property alias echoMode: input.echoMode
    property alias what: label.text
    property alias buttonText: acceptButton.text
    property alias inputMethodHints: input.inputMethodHints
    function clear() {
        input.text = '';
    }
    onFocusChanged: {
        if (focus)
            input.focus = true;
    }
    onClosing: window.hideLineInput()
    ThemedText {
        id: label
        text: {
            if (mode === 'tlog')
                'Тлог или поток';
            else if (mode === 'rating')
                'Минимальный рейтинг';
            else if (mode === 'query')
                'Поиск здесь';
            else if (mode === 'save')
                'Название изображения';
        }
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: contentHeight
    }
    LineInput {
        id: input
        anchors {
            top: label.bottom
            left: parent.left
            right: parent.right
        }
        onAccepted: {
            if (acceptButton.enabled)
                window.handleInputLine(mode, input.text);
        }
        inputMethodHints: {
            if (mode === 'tlog')
                Qt.ImhNoPredictiveText | Qt.ImhPreferLowercase;
            else if (mode === 'rating')
                Qt.ImhDigitsOnly;
            else if (mode === 'query' || mode === 'save')
                Qt.ImhNone;
        }
    }
    ThemedButton {
        id: acceptButton
        anchors {
            top: input.bottom
            right: parent.right
            left: parent.left
        }
        width: window.width / 5
        text: {
            if (mode === 'tlog')
                'Перейти';
            else if (mode === 'rating')
                'Показать';
            else if (mode === 'query')
                'Искать';
            else if (mode === 'save')
                'Сохранить';
        }
        enabled: input.text
        onClicked: window.handleInputLine(mode, input.text)
    }
}
