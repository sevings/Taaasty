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

Rectangle {
    id: scrollbar
    anchors {
        right: parent.right
        margins: 0.5 * mm
    }
    property Flickable flick
    y: flick.visibleArea.yPosition * (flick.height - height + h)
    width: 0.5 * mm
    readonly property int h: flick.visibleArea.heightRatio * flick.height
    height: Math.max(5 * mm, h)
    color: Material.foreground
    opacity: flick.movingVertically ? 0.7 : 0
    visible: opacity > 0 && height < flick.height * 0.9
    Behavior on opacity {
        NumberAnimation { duration: 500 }
    }
    Behavior on height {
        NumberAnimation { duration: 500 }
    }
    radius: width / 2
}
