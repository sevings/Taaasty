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

Pane {
    id: back
    readonly property bool isTrainingProgress: true
    Poppable {
        body: back
        shiftable: false
    }
    Splash {
        visible: tlogBar.to <= 0
    }
    ThemedProgressBar {
        id: tlogBar
        visible: to > 0
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            right: parent.right
        }
        text: Trainer.currentName || 'Тлог'
        value: Trainer.trainedEntriesCount
        to: Trainer.entriesCount
    }
}
