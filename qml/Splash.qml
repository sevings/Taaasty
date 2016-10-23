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
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.3

ColumnLayout {
    spacing: 2 * mm
    property bool running: true
    property string text: ''
    anchors.centerIn: parent
    Q.BusyIndicator {
        running: parent.visible && parent.running
        Layout.alignment: Qt.AlignHCenter
    }
    Q.Label {
        id: loadingText
        Layout.alignment: Qt.AlignHCenter
        Layout.maximumWidth: window.width - 3 * mm
        font.pointSize: window.fontBigger
        wrapMode: Text.Wrap
        horizontalAlignment: Text.AlignHCenter
        text: parent.running ? 'Загрузка…' : parent.text
    }
}

