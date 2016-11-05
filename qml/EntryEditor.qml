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
import QtQuick.Controls 2.0 as Q
import QtQuick.Controls.Material 2.0
import org.binque.taaasty 1.0

Pane {
    id: back
    Component.onCompleted: {
        titleInput.text = Settings.lastTitle;
        textInput.text  = Settings.lastText;

        titleInput.forceActiveFocus();
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
        onVerticalVelocityChanged: editMenu.hideMenu()
        Poppable {
            body: back
        }
        TextEditor {
            id: titleInput
            z: 6
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

                window.hideFooter();
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
            z: 5
            anchors {
                top: titleLine.bottom
            }
            height: Math.max(contentHeight + topPadding + bottomPadding,
                             flick.height - titleInput.height - postButton.height - 3 * mm)
            flickable: flick
            handler: textHandler
//            textFormat: TextEdit.RichText
            placeholderText: 'Текст поста'
            popBody: back
            onActiveFocusChanged: {
                if (!activeFocus)
                    return;

                editMenu.textEdit = textInput;
                editMenu.handler  = textHandler;

                window.hideFooter();
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
        Q.ComboBox {
            id: whereBox
            anchors {
                verticalCenter: postButton.verticalCenter
                left: parent.left
                right: fireButton.left
            }
            model: ListModel {
                ListElement { tlog: 0;  text: "В мой тлог" }
                ListElement { tlog: -1; text: "В анонимки" }
            }
            delegate: Q.MenuItem {
                width: whereBox.width
                text: model[whereBox.textRole]
                Material.foreground: whereBox.currentIndex === index ? whereBox.Material.accent : whereBox.Material.foreground
                highlighted: whereBox.highlightedIndex === index
                font.pointSize: window.fontSmaller
            }
            textRole: "text"
            readonly property int tlog: model.get(currentIndex).tlog
            font.pointSize: window.fontSmaller
        }
        IconButton {
            id: fireButton
            property bool voting: true
            anchors {
                top: textLine.bottom
                right: lockButton.left
            }
            visible: whereBox.tlog >= 0 && !lockButton.locked
            icon: (voting ? '../icons/flame-solid-'
                          : '../icons/flame-outline-')
                  + '72.png'
            onClicked: {
                editMenu.hideMenu();
                voting = !voting;
            }
        }
        IconButton {
            id: lockButton
            property bool locked: false
            anchors {
                top: textLine.bottom
                right: postButton.left
            }
            visible: whereBox.tlog == 0
            icon: (locked ? (window.darkTheme ? '../icons/lock-white-'
                                              : '../icons/lock-black-')
                          : (window.darkTheme ? '../icons/unlock-white-'
                                              : '../icons/unlock-black-'))
                  + '128.png'
            onClicked: {
                editMenu.hideMenu();
                locked = !locked;
            }
        }
        IconButton {
            id: postButton
            anchors {
                top: textLine.bottom
                right: parent.right
            }
            enabled: titleInput.length || textInput.length
            icon: (window.darkTheme ? '../icons/send-light-'
                                    : '../icons/send-dark-')
                  + '128.png'
            onClicked: {
                editMenu.hideMenu();

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
        z: 7
        flickable: flick
        textEdit: titleInput
        handler: titleHandler
        boundItem: bound
    }
    Item {
        id: bound
        width: parent.width
        height: window.footerY
        enabled: false
    }
}
