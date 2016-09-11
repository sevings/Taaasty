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

import QtQuick 2.0
import org.binque.taaasty 1.0

Rectangle {
    id: back
    color: window.backgroundColor
    onPopped: window.popFromStack()
    signal popped
    property bool poppable
    property string where: ''
    Component.onCompleted: {
        titleInput.text = Settings.lastTitle;
        textInput.text  = Settings.lastText;
    }
    Poppable {
        body: back
    }
    LineInput {
        id: titleInput
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
    }
    TextEditor {
        id: textInput
        anchors {
            top: titleInput.bottom
            bottom: formatButtons.top
            left: parent.left
            right: parent.right
        }
    }
    Row {
        id: formatButtons
        anchors {
            left: parent.left
            right: parent.right
            bottom: post.top
            margins: 1 * mm
        }
        spacing: 1 * mm
        ThemedButton {
            id: italic
            width: (parent.width - 5 * 3) / 4
            height: 6 * mm
            text: '<i>I</i>'
            onClicked: textInput.insertTags('<i>', '</i>')
        }
        ThemedButton {
            width: italic.width
            height: italic.height
            text: '<b>B</b>'
            onClicked: textInput.insertTags('<b>', '</b>')
        }
        ThemedButton {
            width: italic.width
            height: italic.height
            text: '<u>U</u>'
            onClicked: textInput.insertTags('<u>', '</u>')
        }
        ThemedButton {
            width: italic.width
            height: italic.height
            text: 'http'
            onClicked: textInput.insertTags('<a href=\"', '\"></a>')
        }
    }
    ThemedButton {
        id: post
        anchors {
            bottom: notNow.top
            left: parent.left
            right: parent.right
        }
        text: 'Отправить ' + back.where
        onClicked: {
//            Settings.lastTitle = '';
//            Settings.lastText  = '';
        }
    }
    ThemedButton {
        id: notNow
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        text: 'Позже'
        onClicked: {
            Settings.lastTitle = titleInput.text;
            Settings.lastText  = textInput.text;
            back.popped();
        }
    }
}
