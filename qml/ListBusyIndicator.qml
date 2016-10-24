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

import QtQuick 2.0
import QtQuick.Controls 2.0 as Q

Item {
    property alias running: busy.running
    property bool footer: true
    readonly property int visibleHeight: footer ? busy.height + 3 * mm
                                                : busy.height
    height: visible ? visibleHeight : 0
    width: window.width
//    visible: running
    Q.BusyIndicator {
        id: busy
        height: 7 * mm
        width: height
        anchors {
            top: footer ? parent.top : undefined
            bottom: footer ? undefined : parent.bottom
            horizontalCenter: parent.horizontalCenter
        }
    }
}
