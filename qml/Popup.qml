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
import QtQuick.Controls.Material.impl 2.0

Rectangle {
    id: popup
    anchors {
        left: parent.left
        right: parent.right
    }
    z: 20
    visible: y < window.height || !closable
    enabled: visible
    property bool closable: true
    signal opened
    signal closing
    color: Material.primary
    onVisibleChanged: {
        if (!closable)
            return;

        if (visible) {
            opened();
            enabled = true;
            popup.forceActiveFocus();
        }
        else {
            enabled = false;
            focus = false;
        }
    }
    Keys.onBackPressed: {
        if (!closable) {
            event.accepted = !window.mayBeClose();
            return;
        }

        closing();
        event.accepted = true;
    }
    layer.enabled: visible && closable
    layer.effect: ElevationEffect {
        elevation: 24
    }
    state: "closed"
    states: [
        State {
            name: "opened"
            AnchorChanges {
                target: popup
                anchors {
                    top: undefined
                    bottom: parent.bottom
                }
            }
        },
        State {
            name: "closed"
            AnchorChanges {
                target: popup
                anchors {
                    top: parent.bottom
                    bottom: undefined
                }
            }
        }
    ]
    transitions: [
        Transition {
            from: "opened"
            to: "closed"
            AnchorAnimation {
                duration: 200
            }
        },
        Transition {
            from: "closed"
            to: "opened"
            AnchorAnimation {
                duration: 200
            }
        }
    ]
    MouseArea {
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.top
        }
        height: parent.y
        visible: closable && parent.visible
        onClicked: closing()
    }
    MouseArea {
        anchors.fill: parent
        visible: closable
        anchors.margins: -2 * mm
    }
}
