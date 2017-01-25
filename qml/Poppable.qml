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

MouseArea {
    property Pane body
    signal popped
    anchors.fill: parent
    drag.target: body
    drag.axis: Drag.XAxis
    drag.minimumX: 0
    drag.maximumX: body ? body.width * 0.9 : 0
    drag.threshold: Math.max(Qt.styleHints.startDragDistance, 3 * mm)
    property bool shiftable: true
    property int bodyX: 0
    property int prevBodyX: 0
    Connections {
        target: body
        onXChanged: {
            prevBodyX = bodyX;
            bodyX = body ? body.x : 0;
        }
    }
    onReleased: {
        if (!body)
            return;

        if (body.poppable)
        {
            if (bodyX > prevBodyX)
                body.popped();
            else
                backAnimation.start();
        }
        else
        {
            if (bodyX > prevBodyX && shiftable)
                forwardAnimation.start();
            else
                backAnimation.start();
        }
    }
    onClicked: {
        if (!body)
            return;

        if (body.x <= 0)
            mouse.accepted = false;
        else {
            backAnimation.start();
            mouse.accepted = true;
        }
    }
    onDoubleClicked: {
        // supress second click
        mouse.accepted = true;
    }
}
