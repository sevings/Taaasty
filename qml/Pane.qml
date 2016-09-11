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
import QtGraphicalEffects 1.0

Rectangle {
    id: back
    color: window.backgroundColor
    property Flickable innerFlick
    property bool poppable
    signal popped
    onPopped: window.popFromStack()
    Rectangle {
        id: pane
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
        }
        color: back.color
        width: 3 * mm
    }
    DropShadow {
        enabled: !innerFlick || !innerFlick.movingVertically
        anchors.fill: pane
        horizontalOffset: -2 * mm
        radius: 1 * mm
        samples: 5
        color: "#40000000"
        source: pane
    }
}


