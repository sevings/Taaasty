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

import QtQuick 2.9
import QtQuick.Controls 2.2 as Q
import QtQuick.Controls.Material 2.2
import org.binque.taaasty 1.0

Pane {
    id: back
    hasMenu: !editEntry
    innerFlick: flick
    readonly property bool isEntryEditor: true
    property int entryType: Settings.lastEntryType;
    readonly property int privacy: lockButton.locked && lockButton.visible
                                   ? Poster.Private : fireButton.voting
                                     ? Poster.Voting : Poster.Public;
    property TlogEntry editEntry
    Component.onCompleted: load()
    Component.onDestruction: save()
    onHeightChanged: {
        if (titleInput.activeFocus)
            titleInput.ensureVisible(titleInput.cursorRectangle);
        else if (textInput.activeFocus)
            textInput.ensureVisible(textInput.cursorRectangle);
        else if (sourceInput.activeFocus)
            sourceInput.ensureVisible(sourceInput.cursorRectangle)
    }
    function load() {
        if (editEntry)
        {
            back.entryType    = editEntry.type;
            titleInput.text   = editEntry.title;
            textInput.text    = editEntry.media ? editEntry.title : editEntry.text;
            sourceInput.text  = editEntry.source;
            urlText.text      = editEntry.videoUrl;
            fireButton.voting = editEntry.isVotable;
            lockButton.locked = editEntry.isPrivate;

            return;
        }

        titleInput.text   = Settings.lastTitle;
        textInput.text    = Settings.lastText;
        sourceInput.text  = Settings.lastSource;
        urlText.text      = Settings.lastUrl;
        fireButton.voting = Settings.lastPrivacy == Poster.Voting;
        lockButton.locked = Settings.lastPrivacy == Poster.Private;

        whereBox.setLastTlog();
    }
    function save() {
        if (editEntry)
            return;

        Settings.lastTitle       = titleInput.text;
        Settings.lastText        = textInput.text;
        Settings.lastSource      = sourceInput.text;
        Settings.lastUrl         = urlText.text;
        Settings.lastPrivacy     = back.privacy;
        Settings.lastEntryType   = back.entryType;
        Settings.lastPostingTlog = whereBox.tlog;
    }
    function clear() {
        titleInput.clear();
        textInput.clear();
        sourceInput.clear();
        urlText.clear();
        window.popFromStack();
    }
    Timer {
        running: !poster.loading && !editEntry
        interval: 30000
        repeat: true
        onTriggered: {
            save();
        }
    }
    Poppable {
        body: back
    }
    MyFlickable {
        id: flick
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        height: Math.min(back.height, contentHeight + 1.5 * mm)
        bottomMargin: 1.5 * mm
        contentWidth: parent.width
        contentHeight: column.height
        onContentHeightChanged: {
            if (!poster.loading)
                returnToBounds();
        }
        Poppable {
            body: back
        }
        Column {
            id: column
            width: window.width
            spacing: 1.5 * mm
            UploadView {
                id: images
                visible: entryType == TlogEntry.ImageEntry
                model: poster.images
                interactive: false
                editable: !poster.loading
            }
            Item {
                id: urlItem
                visible: entryType == TlogEntry.VideoEntry
                width: column.width
                height: Math.max(urlText.height, pasteUrlButton.height)
                Q.TextField {
                    id: urlText
                    anchors {
                        top: parent.top
                        left: parent.left
                        right: pasteUrlButton.left
                        margins: 1.5 * mm
                        topMargin: 0
                    }
                    placeholderText: 'Вставьте ссылку'
                    font.pixelSize: window.fontNormal
                }
                IconButton {
                    id: pasteUrlButton
                    anchors {
                        verticalCenter: parent.verticalCenter
                        right: parent.right
                    }
                    enabled: urlText.canPaste
                    icon: (Settings.darkTheme ? '../icons/paste-white'
                                              : '../icons/paste-black')
                          + '-128.png'
                    onClicked: {
                        urlText.selectAll();
                        urlText.paste();
                    }
                }
            }
            TextEditor {
                id: titleInput
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 3 * mm
                visible: entryType == TlogEntry.TextEntry || entryType == TlogEntry.AnonymousEntry
                z: 6
                readOnly: poster.loading
                flickable: flick
                placeholderText: 'Заголовок'
                font.pixelSize: window.fontBigger
                popBody: back
                onActiveFocusChanged: {
                    if (!activeFocus)
                        return;

                    window.hideFooter();
                }
            }
            TextEditor {
                id: textInput
                z: 5
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 3 * mm
                height: {
                    switch (entryType)
                    {
                    case TlogEntry.ImageEntry:
                    case TlogEntry.VideoEntry:
                        implicitHeight;
                        break;
                    case TlogEntry.QuoteEntry:
                        Math.max(implicitHeight, back.height - sourceInput.height - buttonsRow.height - 6 * mm)
                        break;
                    case TlogEntry.TextEntry:
                    case TlogEntry.AnonymousEntry:
                        Math.max(implicitHeight, back.height - titleInput.height - buttonsRow.height - 6 * mm)
                        break;
                    default:
                        console.log('entry type', entryType);
                    }
                }
                readOnly: poster.loading
                flickable: flick
                textFormat: TextEdit.RichText
                placeholderText: {
                    switch (entryType)
                    {
                    case TlogEntry.ImageEntry:
                    case TlogEntry.VideoEntry:
                        'Подпись'; break;
                    case TlogEntry.QuoteEntry:
                        'Цитата'; break;
                    case TlogEntry.TextEntry:
                        'Текст поста'; break;
                    case TlogEntry.AnonymousEntry:
                        'Текст анонимки'; break;
                    default:
                        console.log('entry type', entryType);
                    }
                }
                popBody: back
                onActiveFocusChanged: {
                    if (!activeFocus)
                        return;

                    window.hideFooter();
                }
            }
            TextEditor {
                id: sourceInput
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 3 * mm
                visible: entryType == TlogEntry.QuoteEntry
                z: 4
                readOnly: poster.loading
                flickable: flick
                placeholderText: 'Источник'
                horizontalAlignment: Text.AlignRight
                popBody: back
                onActiveFocusChanged: {
                    if (!activeFocus)
                        return;

                    window.hideFooter();
                }
            }
            Item {
                id: footerItem
                visible: !poster.loading
                width: column.width
                height: Math.max(whereBox.height, buttonsRow.height)
                Row {
                    id: buttonsRow
                    spacing: 1.5 * mm
                    anchors {
                        verticalCenter: parent.verticalCenter
                        right: parent.right
                        leftMargin: 1.5 * mm
                        rightMargin: 1.5 * mm
                    }
                    IconButton {
                        id: fireButton
                        property bool voting
                        visible: entryType !== TlogEntry.AnonymousEntry
                                 && (!lockButton.locked || whereBox.currentIndex > 0)
                        icon: (voting ? '../icons/flame-solid-'
                                      : '../icons/flame-outline-')
                              + '72.png'
                        onClicked: {
                            voting = !voting;
                        }
                    }
                    IconButton {
                        id: lockButton
                        property bool locked
                        visible: entryType !== TlogEntry.AnonymousEntry && whereBox.currentIndex == 0
                        icon: (locked ? (window.darkTheme ? '../icons/lock-white-'
                                                          : '../icons/lock-black-')
                                      : (window.darkTheme ? '../icons/unlock-white-'
                                                          : '../icons/unlock-black-'))
                              + '128.png'
                        onClicked: {
                            locked = !locked;
                        }
                    }
                    IconButton {
                        id: postButton
                        enabled: {
                            switch (entryType)
                            {
                            case TlogEntry.ImageEntry:
                                images.count;
                                break;
                            case TlogEntry.QuoteEntry:
                                textInput.length;
                                break;
                            case TlogEntry.VideoEntry:
                                urlText.length;
                                break;
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
                            if (editEntry)
                                switch (editEntry.type)
                                {
                                case TlogEntry.ImageEntry:
                                    poster.putImage(editEntry.id, textInput.text, back.privacy);
                                    break;
                                case TlogEntry.QuoteEntry:
                                    poster.putQuote(editEntry.id, textInput.text, sourceInput.text, back.privacy);
                                    break;
                                case TlogEntry.VideoEntry:
                                    poster.putVideo(editEntry.id, textInput.text, urlText.text, back.privacy);
                                    break;
                                case TlogEntry.TextEntry:
                                    poster.putText(editEntry.id, titleInput.text, textInput.text, back.privacy);
                                    break;
                                case TlogEntry.AnonymousEntry:
                                    poster.putAnonymous(editEntry.id, titleInput.text, textInput.text);
                                    break;
                                default:
                                    console.log('entry type', entryType);
                                }
                            else
                                switch (entryType)
                                {
                                case TlogEntry.ImageEntry:
                                    poster.postImage(textInput.text, back.privacy, whereBox.tlog);
                                    break;
                                case TlogEntry.QuoteEntry:
                                    poster.postQuote(textInput.text, sourceInput.text, back.privacy, whereBox.tlog);
                                    break;
                                case TlogEntry.VideoEntry:
                                    poster.postVideo(textInput.text, urlText.text, back.privacy, whereBox.tlog);
                                    break;
                                case TlogEntry.TextEntry:
                                    poster.postText(titleInput.text, textInput.text, back.privacy, whereBox.tlog);
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
                Q.ComboBox {
                    id: whereBox
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                        right: buttonsRow.left
                        leftMargin: 1.5 * mm
                        rightMargin: 1.5 * mm
                    }
                    visible: !back.editEntry
                             && back.entryType != TlogEntry.AnonymousEntry
                             && count > 1
                    textRole: "name"
                    font.pixelSize: window.fontSmaller
                    model: AvailableTlogsModel {
                        onFlowsLoaded: {
                            if (whereBox.currentIndex < 0)
                                whereBox.setLastTlog();
                        }
                    }
                    delegate: Q.MenuItem {
                        width: whereBox.width
                        text: model[whereBox.textRole]
                        Material.foreground: whereBox.currentIndex === index ? whereBox.Material.accent : whereBox.Material.foreground
                        highlighted: whereBox.highlightedIndex === index
                        font.pixelSize: window.fontSmaller
                    }
                    readonly property int tlog: model.indexTlog(currentIndex)
                    property bool tlogSet: false
                    function setLastTlog() {
                        if (tlogSet)
                            return;

                        var last = Settings.lastPostingTlog;
                        var i = whereBox.model.tlogIndex(last);
                        if (i < 0)
                            return;

                        whereBox.currentIndex = i;
                        tlogSet = true;
                    }
                    onActivated: {
                        tlogSet = true;
                    }
                }
            }
            ThemedProgressBar {
                id: uploadBar
                visible: poster.loading
                text: 'Отправка'
                units: 'КБ'
                value: poster.kBytesSent
                to: poster.kBytesTotal
            }
        }
    }
    MyScrollIndicator { flick: flick }
    Poster {
        id: poster
        onPosted: {
            back.clear();
            window.showMessage('Запись опубликована', true);
        }
        onEdited: {
            back.clear();
            window.showMessage('Запись отредактирована', true);
        }
    }
}
