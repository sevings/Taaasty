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

Rectangle {
    id: back
    anchors {
        left: parent.left
        right: parent.right
    }
    height: visible ? parent.height : 0
    color: window.backgroundColor
    clip: true
    z: 20
    visible: y < window.height
    readonly property bool showing: state == "opened"
    function show() {
        state = "opened";
    }
    function hide() {
        state = "closed";
    }
    function toggle() {
        if (showing)
            hide();
        else
            show();
    }
    state: "closed"
    states: [
        State {
            name: "opened"
            AnchorChanges {
                target: back
                anchors.top: parent.top
            }
        },
        State {
            name: "closed"
            AnchorChanges {
                target: back
                anchors.top: parent.bottom
            }
        }
    ]
    transitions: [
        Transition {
            from: "opened"
            to: "closed"
            AnchorAnimation {
                duration: 300
            }
        },
        Transition {
            from: "closed"
            to: "opened"
            AnchorAnimation {
                duration: 300
            }
        }
    ]
}
