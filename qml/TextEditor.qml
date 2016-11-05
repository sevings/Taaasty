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
import QtQuick.Controls.Material 2.0
import org.binque.taaasty 1.0

TextEdit {
    id: textEdit
    property Flickable flickable
    property TextHandler handler
    property alias popBody: pop.body
    property string placeholderText: ''
    property bool canCopyPaste: false
    readonly property bool richEditing: textFormat === TextEdit.RichText
    readonly property bool hasSelection: selectedText
    width: flickable.width
    font.pointSize: window.fontNormal
    padding: 1.5 * mm
    text: handler.text
    textFormat:  TextEdit.PlainText
    wrapMode: TextEdit.Wrap
    activeFocusOnPress: false
    selectByMouse: false
    color: enabled ? Material.primaryTextColor : Material.hintTextColor
    selectionColor: Material.accentColor
    selectedTextColor: Material.primaryHighlightedTextColor
    function addGreeting(slug) {
        insert(0, '@' + slug + ', ');
        forceActiveFocus();
    }
    function clear() {
        text = '';
    }
    function ensureVisible(cursor)
    {
        if (flickable.contentY >= textEdit.y + cursor.top)
            flickable.contentY = textEdit.y + cursor.top;
        else if (flickable.contentY + flickable.height <= textEdit.y + cursor.bottom)
            flickable.contentY = textEdit.y + cursor.bottom - flickable.height;
    }
    function setHandlePositions() {
        leftSelectionHandle.setPosition();
        rightSelectionHandle.setPosition();
    }
    onHasSelectionChanged: {
        if (hasSelection)
            canCopyPaste = true;
    }
    onCursorPositionChanged: {
        if (!hasSelection) {
            canCopyPaste = false;
            ensureVisible(cursorRectangle);
        }
    }
    Poppable {
        id: pop
        function moveCursor(x, y) {
            textEdit.deselect();
            textEdit.cursorPosition = textEdit.positionAt(x, y);
            textEdit.forceActiveFocus();
        }
        onPressed: {
            mouse.accepted = true
        }
        onReleased: {
            if (flickable.movingVertically || mouse.wasHeld)
                return;

            moveCursor(mouse.x, mouse.y);
            textEdit.canCopyPaste = true;
        }
        onPressAndHold: {
            moveCursor(mouse.x, mouse.y);
            textEdit.selectWord();
            textEdit.setHandlePositions();

            mouse.accepted = true;
        }
    }
    property SelectionHandle leftSelectionHandle: SelectionHandle {
        textEdit: textEdit
        textPosition: textEdit.selectionStart
        onMoved: {
            if (currentPosition >= textEdit.selectionEnd)
                return;

            textEdit.select(currentPosition, textEdit.selectionEnd);
            textEdit.ensureVisible(textEdit.positionToRectangle(textPosition));
        }
    }
    property SelectionHandle rightSelectionHandle: SelectionHandle {
        textEdit: textEdit
        textPosition: textEdit.selectionEnd
        onMoved: {
            if (currentPosition <= textEdit.selectionStart)
                return;

            textEdit.select(textEdit.selectionStart, currentPosition);
            textEdit.ensureVisible(textEdit.positionToRectangle(textPosition));
        }
    }
    Text {
        id: placeholder
        x: textEdit.leftPadding
        y: textEdit.topPadding
        width: textEdit.width - (textEdit.leftPadding + textEdit.rightPadding)
        height: textEdit.height - (textEdit.topPadding + textEdit.bottomPadding)
        text: textEdit.placeholderText
        font: textEdit.font
        color: Material.hintTextColor
        wrapMode: Text.Wrap
        visible: !textEdit.length && !textEdit.preeditText && !textEdit.activeFocus
    }
}
