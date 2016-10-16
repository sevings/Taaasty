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
import QtQuick.Controls 2.0 as Q

FocusScope {
    id: editor
    implicitWidth: window.width
    implicitHeight: 18 * mm
    property alias message: input.text
    property bool uploading: false
    signal sent
    onActiveFocusChanged: {
        if (focus)
            window.hideFooter();
    }
    function clear() {
        input.clear();
        uploading = false;
    }
    function addGreeting(slug) {
        input.addGreeting(slug);
        editor.focus = true;
    }
    TextEditor {
        id: input
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: button.left
        }
        height: 15 * mm
        focus: true
    }
    IconButton {
        id: button
        anchors {
            bottom: parent.bottom
            right: parent.right
        }
        icon: (window.darkTheme ? '../icons/send-light-'
                                : '../icons/send-dark-')
              + '128.png'
        enabled: !editor.uploading && input.text
        onClicked: {
            editor.uploading = true;
            editor.sent();
        }
    }
    Q.BusyIndicator {
        id: busy
        height: 7 * mm
        width: height
        anchors.centerIn: parent
        running: uploading
    }
}
