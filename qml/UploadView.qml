/*
 * Copyright (C) 2017 Vasily Khodakov
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

import QtQuick 2.9
import org.binque.taaasty 1.0



ListView {
    id: images
    width: parent.width
    spacing: 1.5 * mm
    height: visible ? contentHeight : 0
    flickableDirection: Flickable.VerticalFlick
    maximumFlickVelocity: 1000 * mm
    interactive: parent.x <= 0
    boundsBehavior: Flickable.DragOverBounds
    property bool editable: true
    property Pane popBody
    onVerticalVelocityChanged: {
        if (!dragging)
            return;

        if (verticalVelocity > 0) {
            window.hideFooter();
        }
        else if (verticalVelocity < 0) {
            window.showFooter();
        }
    }
    add: Transition {
        NumberAnimation { property: "opacity"; from: 0;   to: 1.0; duration: 300 }
        NumberAnimation { property: "scale";   from: 0.8; to: 1.0; duration: 300 }
    }
    displaced: Transition {
        NumberAnimation { property: "y"; duration: 300 }
    }
    delegate: AnimatedImage {
        id: picture
        width: images.width
        cache: true
        smooth: true
        asynchronous: false
        autoTransform: true
        fillMode: Image.PreserveAspectFit
        source: model.display
        Poppable {
            body: images.popBody
        }
        Rectangle {
            anchors.fill: removeImageButton
            color: window.backgroundColor
            opacity: 0.3
            radius: width / 2
        }
        IconButton {
            id: removeImageButton
            enabled: editable
            anchors {
                top: parent.top
                right: parent.right
            }
            icon: (window.darkTheme ? '../icons/cross-white'
                                    : '../icons/cross-black')
                  + '-128.png'
            onClicked: removeAnimation.start()
        }
        ParallelAnimation {
            id: removeAnimation
            onStopped: images.model.remove(model.index)
            NumberAnimation { target: picture; property: "opacity"; from: 1.0; to: 0;   duration: 300 }
            NumberAnimation { target: picture; property: "scale";   from: 1.0; to: 0.8; duration: 300 }
        }
    }
    footer: Item {
        width: images.width
        height: addImageButton.height + optimizeBox.height + 4.5 * mm
        Poppable {
            body: images.popBody
        }
        ThemedButton {
            id: addImageButton
            enabled: editable
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }
            width: implicitWidth + 5 * mm
            text: 'Добавить'
            onClicked: images.model.append()
        }
        ThemedCheckBox {
            id: optimizeBox
            enabled: editable
            anchors {
                top: addImageButton.bottom
                left: parent.left
                right: parent.right
                margins: 1.5 * mm
            }
            text: 'Оптимизировать размер'
            onCheckedChanged: { Settings.lastOptimizeImages = checked; }
            checked: Settings.lastOptimizeImages
        }
    }
}
