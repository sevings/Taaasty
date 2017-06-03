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

import QtQuick 2.9
import QtQuick.Controls 2.2 as Q
import QtQuick.Controls.Material 2.2
import ImageCache 2.0

Item {
    id: image
    property url url: ''
    property string extension: ''
    property color backgroundColor: window.darkTheme ? Qt.darker('#9E9E9E') : '#9E9E9E'
    property bool savable: false
    property alias acceptClick: pop.enabled
    property alias popBody: pop.body
    property CachedImage cachedImage
    readonly property bool available: cachedImage && cachedImage.available
    readonly property bool isSmall: (width > 0 && width < 12 * mm)
                                    || (height > 0 && height < 12 * mm)
    signal clicked
    onUrlChanged: setImage()
    onAvailableChanged: {
        if (available)
            return;

        if (cachedImage)
            cachedImage.load(isSmall);
        else
            setImage();
    }
    function setImage() {
        cachedImage = Cache.image(image.url);

        if (!cachedImage.extension)
            cachedImage.extension = image.extension

        cachedImage.load(isSmall);
    }
    Component {
        id: staticImage
        Image {
            cache: false
            smooth: true
            asynchronous: false
            fillMode: Image.PreserveAspectCrop
            source: image.available ? cachedImage.source : ''
        }
    }
    Component {
        id: animatedImage
        AnimatedImage {
            cache: image.isSmall
            smooth: true
            asynchronous: false
            fillMode: Image.PreserveAspectCrop
            source: image.available ? cachedImage.source : ''
        }
    }
    Loader {
        anchors.fill: parent
        asynchronous: false
        active: image.available
        sourceComponent: image.available
                         ? (cachedImage.format == CachedImage.GifFormat
                            ? animatedImage : staticImage)
                         : undefined
        Component.onDestruction: sourceComponent = undefined
    }
    Poppable {
        id: pop
        onClicked: {
            mouse.accepted = acceptClick;
            if (acceptClick)
                image.clicked();
        }
        onPressAndHold: {
            if (image.savable && image.available)
                window.saveImage(cachedImage);
        }
    }
    Loader {
        id: back
        anchors.fill: parent
        active: !image.available
        asynchronous: false
        Component.onDestruction: sourceComponent = undefined
        sourceComponent: Rectangle {
            color: image.backgroundColor
            Rectangle {
                id: downloadButton
                anchors {
                    centerIn: parent
                    bottomMargin: 1.5 * mm
                    alignWhenCentered: false
                }
                visible: image.url && cachedImage && !cachedImage.available && back.width > height && back.height > height
                width: cachedImage && cachedImage.kbytesTotal > 0 ? cachedImage.kbytesReceived * (back.width - height)
                                                                    / cachedImage.kbytesTotal + height
                                                                  : height
                height: 12 * mm
                radius: height / 2
                color: Material.primary
                Behavior on width {
                    NumberAnimation { duration: 100 }
                }
                Behavior on scale {
                    NumberAnimation { easing.overshoot: 5; easing.type: Easing.OutBack; duration: 400; }
                }
                MouseArea {
                    anchors.fill: parent
                    z: image.z + 1
                    onClicked: {
                        mouse.accepted = true;

                        if (!cachedImage)
                            return;

                        if (cachedImage.isDownloading)
                            cachedImage.abortDownload();
                        else
                            cachedImage.download();
                    }
                    onDoubleClicked: {
                        // supress second click
                    }
                    onPressedChanged: {
                        if (pressed)
                            parent.scale = 0.8;
                        else
                            parent.scale = 1;
                    }
                }
                Q.Label {
                    id: bytesText
                    font.pixelSize: window.fontSmallest
                    text: cachedImage ? ((cachedImage.isDownloading && cachedImage.kbytesTotal > 0
                                          ? cachedImage.kbytesReceived + ' / ' : '')
                                         + (cachedImage.kbytesTotal > 0
                                            ? cachedImage.kbytesTotal + ' КБ ' : '')
                                         + (cachedImage.extension ? '\n' + cachedImage.extension : '')) : ''
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                        right: parent.right
                        margins: 1.5 * mm
                    }
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                }
            }
        }
    }
}
