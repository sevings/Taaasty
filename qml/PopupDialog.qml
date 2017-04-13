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

Dialog {
    id: dalog
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    font.pixelSize: window.fontNormal
    modal: true
    width: label.width + 3 * mm
    property alias text: label.text
    property bool isQuestion
    footer: DialogButtonBox {
        standardButtons: isQuestion ? Dialog.Ok | Dialog.Cancel : Dialog.Ok
        alignment: undefined
    }
    ThemedText {
        id: label
        width: Math.min(implicitWidth, window.width - 13 * mm)
    }
}
