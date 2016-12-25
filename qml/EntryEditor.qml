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
    hasMenu: true
    readonly property bool isEntryEditor: true
    property int entryType: Settings.lastEntryType
    readonly property int privacy: lockButton.locked
                                   ? Poster.Private : fireButton.voting
                                     ? Poster.Voting : Poster.Public;
    onEntryTypeChanged: {
        editMenu.hideMenu();
    }
    Component.onCompleted: {
        titleInput.text   = Settings.lastTitle;
        textInput.text    = Settings.lastText;
        fireButton.voting = Settings.lastPrivacy == Poster.Voting;
        lockButton.locked = Settings.lastPrivacy == Poster.Private;

//        var last = Settings.lastPostingTlog;
//        while (whereBox.tlog !== last && whereBox.currentIndex < whereBox.count)
//            whereBox.incrementCurrentIndex();
//        if (whereBox.currentIndex == whereBox.count)
//            whereBox.currentIndex = 0;

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
//        Settings.lastPostingTlog = whereBox.tlog;
        Settings.lastEntryType   = back.entryType;
    }
    Timer {
        running: !poster.loading
        interval: 30000
        repeat: true
        onTriggered: {
            save();
        }
    }
    Poppable {
        body: back
        enabled: !poster.loading
    }
    MyFlickable {
        id: flick
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        height: Math.min(back.height, contentHeight + 1.5 * mm)
        onVerticalVelocityChanged: editMenu.hideMenu()
        bottomMargin: 1.5 * mm
        contentWidth: parent.width
        contentHeight: column.height
        onContentHeightChanged: returnToBounds()
        Poppable {
            body: back
            enabled: !poster.loading
        }
        Column {
            id: column
            width: window.width
            spacing: 1.5 * mm
            ListView {
                id: images
                width: parent.width
                visible: entryType === TlogEntry.ImageEntry
                interactive: false
                spacing: 1.5 * mm
                height: visible ? contentHeight : 0
                model: poster.images
                add: Transition {
                    NumberAnimation { property: "opacity"; from: 0;   to: 1.0; duration: 300 }
                    NumberAnimation { property: "scale";   from: 0.8; to: 1.0; duration: 300 }
                }
                displaced: Transition {
                    NumberAnimation { property: "y"; duration: 300 }
                }
                delegate: AnimatedImage {
                    id: picture
                    width: images.width
                    cache: true
                    smooth: true
                    asynchronous: true
                    fillMode: Image.PreserveAspectFit
                    source: model.display
                    Rectangle {
                        anchors.fill: removeImageButton
                        color: window.backgroundColor
                        opacity: 0.3
                        radius: width / 2
                    }
                    IconButton {
                        id: removeImageButton
                        enabled: !poster.loading
                        anchors {
                            top: parent.top
                            right: parent.right
                        }
                        icon: (window.darkTheme ? '../icons/cross-white'
                                                : '../icons/cross-black')
                              + '-128.png'
                        onClicked: removeAnimation.start()
                    }
                    ParallelAnimation {
                        id: removeAnimation
                        onStopped: poster.images.remove(model.index)
                        NumberAnimation { target: picture; property: "opacity"; from: 1.0; to: 0;   duration: 300 }
                        NumberAnimation { target: picture; property: "scale";   from: 1.0; to: 0.8; duration: 300 }
                    }
                }
                footer: Item {
                    width: images.width
                    height: addImageButton.height + optimizeBox.height + 4.5 * mm
                    ThemedButton {
                        id: addImageButton
                        enabled: !poster.loading
                        anchors {
                            top: parent.top
                            horizontalCenter: parent.horizontalCenter
                        }
                        width: implicitWidth + 5 * mm
                        highlighted: true
                        text: 'Добавить из галереи'
                        onClicked: poster.images.append()
                    }
                    ThemedCheckBox {
                        id: optimizeBox
                        anchors {
                            top: addImageButton.bottom
                            left: parent.left
                            margins: 1.5 * mm
                        }
                        text: 'Оптимизировать размер'
                        onCheckedChanged: { Settings.lastOptimizeImages = checked; }
                        checked: Settings.lastOptimizeImages
                    }
                }
            }
            TextEditor {
                id: titleInput
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 3 * mm
                z: 6
                readOnly: poster.loading
                flickable: flick
                handler: titleHandler
                placeholderText: entryType === TlogEntry.ImageEntry ? 'Подпись' : 'Заголовок'
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
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 3 * mm
                height: Math.max(implicitHeight, back.height - titleInput.height - buttonsRow.height - 6 * mm)
                visible: entryType == TlogEntry.TextEntry || entryType == TlogEntry.AnonymousEntry
                readOnly: poster.loading
                flickable: flick
                handler: textHandler
//                textFormat: TextEdit.RichText
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
            Row {
                id: buttonsRow
                spacing: 1.5 * mm
                anchors {
                    right: parent.right
                    margins: 1.5 * mm
                }
                ThemedProgressBar {
                    id: uploadBar
                    visible: poster.loading
                    width: column.width
                    text: 'Отправка'
                    value: poster.kBytesSent
                    to: poster.kBytesTotal
                }
                IconButton {
                    id: fireButton
                    property bool voting
                    visible: entryType != TlogEntry.AnonymousEntry && !lockButton.locked && !poster.loading
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
                    visible: entryType != TlogEntry.AnonymousEntry && !poster.loading// && to my tlog?
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
                    visible: !poster.loading
                    enabled: {
                        switch (entryType)
                        {
                        case TlogEntry.ImageEntry:
                            images.count;
                            break;
                        case TlogEntry.QuoteEntry:
                        case TlogEntry.VideoEntry:
                        case TlogEntry.TextEntry:
                        case TlogEntry.AnonymousEntry:
                            titleInput.length || textInput.length
                            break;
                        default:
                            console.log('entry type', entryType);
                        }
                    }
                    icon: (window.darkTheme ? '../icons/send-light-'
                                            : '../icons/send-dark-')
                          + '128.png'
                    onClicked: {
                        editMenu.hideMenu();

                        switch (entryType)
                        {
                        case TlogEntry.ImageEntry:
                            poster.postImage(titleInput.text, back.privacy);//, whereBox.tlog);
                            break;
                        case TlogEntry.QuoteEntry:
                            poster.postQuote(titleInput.text, textInput.text, back.privacy);//, whereBox.tlog);
                            break;
                        case TlogEntry.VideoEntry:
                            poster.postVideo(titleInput.text, textInput.text, back.privacy);//, whereBox.tlog);
                            break;
                        case TlogEntry.TextEntry:
                            poster.postText(titleInput.text, textInput.text, back.privacy);//, whereBox.tlog);
                            break;
                        case TlogEntry.AnonymousEntry:
                            poster.postAnonymous(titleInput.text, textInput.text);
                            break;
                        default:
                            console.log('entry type', entryType);
                        }
                    }
                }
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
