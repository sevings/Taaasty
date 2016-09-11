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
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0

Flickable {
    id: flickText
    anchors.margins: 1 * mm
    flickableDirection: Flickable.VerticalFlick
    property alias text: input.text
    onFocusChanged: if (focus) {
                        input.forceActiveFocus();
                    }
    function addGreeting(slug) {
        input.insert(0, '@' + slug + ', ');
        input.forceActiveFocus();
    }
    function insertTags(opening, closing) {
        if (!input.focus)
            return;

        input.insert(input.cursorPosition, opening);
        input.cursorPosition += opening.length;
        if (closing)
            input.insert(input.cursorPosition, closing);
    }
    function clear() {
        input.text = '';
    }
    TextArea.flickable: TextArea {
        id: input
        font.pointSize: window.fontNormal
        wrapMode: TextEdit.Wrap
        textFormat: Text.PlainText
        padding: 1 * mm
        implicitHeight: contentHeight + 2 * mm
        implicitWidth: 30 * mm
    }
}
