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
import org.binque.taaasty 1.0

ListView {
    id: list
    highlightFollowsCurrentItem: false
    cacheBuffer: window.height * 2
    flickableDirection: Flickable.VerticalFlick
    maximumFlickVelocity: 1000 * mm
    interactive: parent.x <= 0
    property bool poppable: true
    boundsBehavior: Flickable.DragOverBounds
    onVerticalVelocityChanged: {
        if (!dragging)
            return;

        if (verticalVelocity > 0) {
            window.hideFooter();
//            upButton.state = "down";
        }
        else if (verticalVelocity < 0) {
            window.showFooter();
//            upButton.state = "up";
        }
    }
    add: Transition {
        NumberAnimation { property: "opacity"; from: 0;   to: 1.0; duration: 300 }
        NumberAnimation { property: "scale";   from: 0.8; to: 1.0; duration: 300 }
    }
    remove: Transition {
        NumberAnimation { property: "opacity"; from: 1.0; to: 0; duration: 300 }
        NumberAnimation { property: "scale";   from: 1.0; to: 0.8; duration: 300 }
    }
    displaced: Transition {
        NumberAnimation { property: "y"; duration: 300 }
    }
    Rectangle {
        id: scrollbar
        anchors {
            right: parent.right
            margins: 0.5 * mm
        }
        y: parent.visibleArea.yPosition * (parent.height - height + h)
        width: 1 * mm
        property int h: parent.visibleArea.heightRatio * parent.height
        height: h > 5 * mm ? h : 5 * mm
        color: Material.foreground
        opacity: parent.movingVertically ? 0.7 : 0
        visible: opacity > 0 && height < parent.height * 0.9
        Behavior on opacity {
            NumberAnimation { duration: 500 }
        }
        Behavior on height {
            NumberAnimation { duration: 500 }
        }
        radius: 0.5 * mm
    }
/*    Rectangle {
        id: upButton
        anchors {
            right: parent.right
            margins: 1 * mm
        }
        width: 10 * mm
        height: width
        radius: width / 2
        color: window.textColor
        opacity: 0.3
        visible: y < window.height
        Text {
            anchors.centerIn: parent
            font.pointSize: window.fontBiggest
            text: '^'
            color: window.textColor
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                upButton.state = "down";
                list.positionViewAtBeginning();
            }
        }
        state: "down"
        states: [
            State {
                name: "up"
                PropertyChanges {
                    target: upButton
                    y: window.height - height - 10 * mm
                }
            },
            State {
                name: "down"
                PropertyChanges {
                    target: upButton
                    y: window.height
                }
            }
        ]
        transitions: [
            Transition {
                NumberAnimation {
                    easing.type: Easing.OutCubic
                    duration: 400
                    property: 'y'
                }
            }
        ]
    }*/
}
