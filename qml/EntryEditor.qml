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
    readonly property int privacy: lockButton.locked
                                   ? Poster.Private : fireButton.voting
                                     ? Poster.Voting : Poster.Public;
    Component.onCompleted: {
        titleInput.text   = Settings.lastTitle;
        textInput.text    = Settings.lastText;
        fireButton.voting = Settings.lastPrivacy == Poster.Voting;
        lockButton.locked = Settings.lastPrivacy == Poster.Private;

        var last = Settings.lastPostingTlog;
        while (whereBox.tlog !== last && whereBox.currentIndex < whereBox.count)
            whereBox.incrementCurrentIndex();
        if (whereBox.currentIndex == whereBox.count)
            whereBox.currentIndex = 0;

        titleInput.forceActiveFocus();
    }
    Component.onDestruction: {
        save();
    }
    onHeightChanged: {
        if (titleInput.activeFocus)
            titleInput.ensureVisible(titleInput.cursorRectangle);
        else if (textInput.activeFocus)
            textInput.ensureVisible(textInput.cursorRectangle);
    }
    function save() {
        Settings.lastTitle       = titleInput.text;
        Settings.lastText        = textInput.text;
        Settings.lastPrivacy     = back.privacy;
        Settings.lastPostingTlog = whereBox.tlog;
    }
    Timer {
        running: back.visible
        interval: 30000
        repeat: true
        onTriggered: {
            save();
        }
    }
    Splash {
        visible: poster.loading
    }
    MyFlickable {
        id: flick
        anchors.fill: parent
        visible: !poster.loading
        contentWidth: Math.max(Math.max(titleInput.contentWidth, textInput.contentWidth), window.width)
        contentHeight: Math.max(parent.height, titleInput.implicitHeight + textInput.implicitHeight + postButton.height + 6 * mm)
        onVerticalVelocityChanged: editMenu.hideMenu()
        Poppable {
            body: back
        }
        TextEditor {
            id: titleInput
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                margins: 1.5 * mm
            }
            z: 6
            flickable: flick
            handler: titleHandler
            placeholderText: 'Заголовок'
            font.pixelSize: window.fontBigger
            popBody: back
            onActiveFocusChanged: {
                if (!activeFocus)
                    return;

                editMenu.textEdit = titleInput;
                editMenu.handler  = titleHandler;

                window.hideFooter();
            }
        }
        TextEditor {
            id: textInput
            z: 5
            anchors {
                top: titleInput.bottom
                bottom: postButton.top
                left: parent.left
                right: parent.right
                margins: 1.5 * mm
            }
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
        Q.ComboBox {
            id: whereBox
            anchors {
                verticalCenter: postButton.verticalCenter
                left: parent.left
                right: fireButton.left
                margins: 1.5 * mm
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
                font.pixelSize: window.fontSmaller
            }
            textRole: "text"
            readonly property int tlog: model.get(currentIndex).tlog
            font.pixelSize: window.fontSmaller
        }
        IconButton {
            id: fireButton
            property bool voting
            anchors {
                bottom: parent.bottom
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
            property bool locked
            anchors {
                bottom: parent.bottom
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
                bottom: parent.bottom
                right: parent.right
            }
            enabled: titleInput.length || textInput.length
            icon: (window.darkTheme ? '../icons/send-light-'
                                    : '../icons/send-dark-')
                  + '128.png'
            onClicked: {
                editMenu.hideMenu();
                poster.postText(titleInput.text, textInput.text, back.privacy, whereBox.tlog);
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
    Poster {
        id: poster
        onPosted: {
            titleInput.clear();
            textInput.clear();
            window.popFromStack();
            window.showMessage('Запись опубликована', true);
        }
    }
}
