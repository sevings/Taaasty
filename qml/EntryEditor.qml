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
import QtQuick.Controls.Material 2.0
import org.binque.taaasty 1.0

Pane {
    id: back
    property string where: ''
    Component.onCompleted: {
        titleInput.text = Settings.lastTitle;
        textInput.text  = Settings.lastText;

        titleInput.forceActiveFocus()
    }
    Component.onDestruction: {
        save();
    }
    Timer {
        running: back.visible
        interval: 30000
        repeat: true
        onTriggered: {
            save();
        }
    }
    function save() {
        Settings.lastTitle = titleInput.text;
        Settings.lastText  = textInput.text;
    }
    MyFlickable {
        id: flick
        anchors.fill: parent
        contentWidth: Math.max(Math.max(titleInput.contentWidth, textInput.contentWidth), window.width)
        contentHeight: postButton.y + postButton.height + 1.5 * mm
        Poppable {
            body: back
        }
        TextEditor {
            id: titleInput
            height: contentHeight + topPadding + bottomPadding
            flickable: flick
            handler: titleHandler
            placeholderText: 'Заголовок'
            font.pointSize: window.fontBigger
            popBody: back
            onActiveFocusChanged: {
                if (!activeFocus)
                    return;

                editMenu.textEdit = titleInput;
                editMenu.handler  = titleHandler;
            }
        }
        Rectangle {
            id: titleLine
            anchors {
                bottom: titleInput.bottom
                left: titleInput.left
                right: titleInput.right
                leftMargin: 1.5 * mm
                rightMargin: anchors.leftMargin
            }
            height: titleInput.activeFocus ? 2 * sp : 1 * sp
            color: titleInput.activeFocus ? Material.accentColor : Material.hintTextColor
        }
        TextEditor {
            id: textInput
            anchors {
                top: titleLine.bottom
            }
            height: Math.max(contentHeight + topPadding + bottomPadding,
                             flick.height - titleInput.height - postButton.height - 3 * mm)
            flickable: flick
            handler: textHandler
            textFormat: TextEdit.RichText
            placeholderText: 'Текст поста'
            popBody: back
            onActiveFocusChanged: {
                if (!activeFocus)
                    return;

                editMenu.textEdit = textInput;
                editMenu.handler  = textHandler;
            }
        }
        Rectangle {
            id: textLine
            anchors {
                bottom: textInput.bottom
                left: textInput.left
                right: textInput.right
                leftMargin: 1.5 * mm
                rightMargin: anchors.leftMargin
            }
            height: textInput.activeFocus ? 2 * sp : 1 * sp
            color: textInput.activeFocus ? Material.accentColor : Material.hintTextColor
        }
        ThemedButton {
            id: postButton
            anchors {
                top: textLine.bottom
                horizontalCenter: parent.horizontalCenter
            }
            implicitWidth: 40 * mm
            highlighted: true
            text: 'Отправить ' + back.where
            onClicked: {
                titleInput.clear();
                textInput.clear();
            }
        }
    }
    TextHandler {
        id: titleHandler
        target: titleInput
        cursorPosition: titleInput.cursorPosition
        selectionStart: titleInput.selectionStart
        selectionEnd: titleInput.selectionEnd
        onError: {
            console.error(message);
        }
    }
    TextHandler {
        id: textHandler
        target: textInput
        cursorPosition: textInput.cursorPosition
        selectionStart: textInput.selectionStart
        selectionEnd: textInput.selectionEnd
        onError: {
            console.error(message);
        }
    }
    TextEditorMenu {
        id: editMenu
        flickable: flick
        textEdit: titleInput
        handler: titleHandler
    }
    /*
    Row {
        id: formatButtons
        anchors {
            left: parent.left
            right: parent.right
            bottom: post.top
            margins: 1.5 * mm
        }
        spacing: 1.5 * mm
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
*/
}
