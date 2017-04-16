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
import QtQuick.Controls.Material 2.1
import org.binque.taaasty 1.0
import QtQuick.Window 2.2

MyImage {
    id: avatar
    anchors {
        top: parent.top
        left: parent.left
    }
    width: 8 * mm
    height: width
    property User user
    property string symbol: user && user.name ? user.name[0].toUpperCase() : '?'
    property url defaultSource: ''
    url: user && (Screen.pixelDensity <= 8 ? user.thumb64 : user.thumb128) || defaultSource
    backgroundColor: user && user.backgroundColor || '#373737'
    Text {
        id: letter
        visible: !avatar.available
        anchors.centerIn: parent
        color: user && user.nameColor || Material.foreground
        font.pixelSize: avatar.width / 2
        text: avatar.symbol.toUpperCase()
    }
}
