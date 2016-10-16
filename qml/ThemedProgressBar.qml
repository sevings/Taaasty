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
    id: control
    spacing: 1.5 * mm
    property string text: ''
    property bool percents: false
    property alias to: bar.to
    property int value: 0
    property int showValue: value
    Behavior on showValue {
        NumberAnimation { duration: 200 }
    }
    Q.ProgressBar {
        id: bar
        Layout.preferredWidth: window.width
        padding: 1.5 * mm
        value: control.showValue
    }
    ThemedText {
        id: label
        leftPadding: 5 * mm
        rightPadding: leftPadding
        Layout.preferredWidth: window.width
        text: control.text + (control.showValue <= control.to && control.to > 0
                              ? ' — ' + (control.percents ? Math.round(control.showValue / control.to * 100) + '%'
                                                          : control.showValue + '/' + control.to) : '')
    }
}
