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

import QtQuick 2.8
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import org.binque.taaasty 1.0

TextArea {
    id: textEdit
    property Flickable flickable
    property alias popBody: pop.body
    font.pixelSize: window.fontNormal
//    text: handler.text
    textFormat:  TextEdit.PlainText
    wrapMode: TextEdit.Wrap
    activeFocusOnPress: true
    persistentSelection: true
    selectByMouse: false
    inputMethodHints: Settings.predictiveText ? Qt.ImhNone : Qt.ImhNoPredictiveText
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
    onCursorPositionChanged: {
        if (!selectedText) {
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

            if (!Qt.inputMethod.visible)
                Qt.inputMethod.show()

            moveCursor(mouse.x, mouse.y);
            mouse.accepted = true;
        }
        onPressAndHold: {
            moveCursor(mouse.x, mouse.y);
            textEdit.selectWord();

            mouse.accepted = true;
        }
    }
    TextHandler {
        id: handler
        target: textEdit
        cursorPosition: textEdit.cursorPosition
        selectionStart: textEdit.selectionStart
        selectionEnd: textEdit.selectionEnd
        onError: {
            console.error(message);
        }
    }
}
