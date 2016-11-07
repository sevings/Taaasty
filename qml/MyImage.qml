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
import ImageCache 2.0

Loader {
    id: image
    asynchronous: true
    property string url: ''
    property string extension: ''
    property alias backgroundColor: back.color
    property bool savable: false
    property bool acceptClick: true
    property alias popBody: pop.body
    property CachedImage cachedImage: Cache.image()
    signal available
    signal clicked
    Component.onCompleted: {
        if (cachedImage.available)
            showImage();
        else if ((width > 0 && width < 12 * mm)
                || (height > 0 && height < 12 * mm)) {
            cachedImage.download();
        }
    }
    Component.onDestruction: {
        image.sourceComponent = undefined;
    }
    onUrlChanged: {
        cachedImage = Cache.image(image.url);

        if (!cachedImage.extension)
            cachedImage.extension = image.extension

        if (cachedImage.available)
            showImage();
        else
            hideImage();
    }
    onLoaded: {
        item.source = cachedImage.source
    }
    function showImage() {
        back.visible = false;
        image.sourceComponent = cachedImage.format == CachedImage.GifFormat
                ? animatedImage : staticImage
        if (image.status === Loader.Ready)
            item.source = cachedImage.source;
        image.available()
    }
    function hideImage() {
        back.visible = true;
        image.sourceComponent = undefined;
    }
    Connections {
        target: cachedImage
        onAvailable: {
            showImage();
        }
    }
    Component {
        id: animatedImage
        AnimatedImage {
            cache: true
            smooth: true
            asynchronous: true
            fillMode: Image.PreserveAspectCrop
        }
    }
    Component {
        id: staticImage
        Image {
            cache: true
            smooth: true
            asynchronous: true
            fillMode: Image.PreserveAspectCrop
        }
    }
    Poppable {
        id: pop
        propagateComposedEvents: !acceptClick
        onClicked: {
            mouse.accepted = acceptClick;
            if (acceptClick)
                image.clicked();
        }
        onPressAndHold: {
            if (image.savable && cachedImage.available)
                window.saveImage(cachedImage);
        }
    }
    Rectangle {
        id: back
        anchors.fill: parent
        color: window.darkTheme ? Qt.darker('#9E9E9E') : '#9E9E9E'
        Loader {
            active: image.url && !cachedImage.available && back.width > 12 * mm && back.height > 12 * mm
            anchors {
                verticalCenter: parent.verticalCenter
                horizontalCenter: parent.horizontalCenter
                bottomMargin: 1.5 * mm
                alignWhenCentered: false
            }
            sourceComponent: Rectangle {
                id: downloadButton
                width: cachedImage.kbytesTotal > 0 ? cachedImage.kbytesReceived * (back.width - 12 * mm)
                                                     / cachedImage.kbytesTotal + 12 * mm
                                                   : 12 * mm
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
                    onClicked: {
                        mouse.accepted = true;

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
                    font.pointSize: window.fontSmallest
                    text: (cachedImage.isDownloading && cachedImage.kbytesTotal > 0
                           ? cachedImage.kbytesReceived + ' / ' : '')
                          + (cachedImage.kbytesTotal > 0
                             ? cachedImage.kbytesTotal + ' KB ' : '')
                          + (cachedImage.extension ? '\n' + cachedImage.extension : '')
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
