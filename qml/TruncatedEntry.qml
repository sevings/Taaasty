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
import org.binque.taaasty 1.0

Item {
    id: truncEntry
    height: wc.y + wc.height
    property TlogEntry entry
    property Pane popBody
    property alias pinVisible: pin.active
    signal avatarClicked
    SmallAvatar {
        id: entryAvatar
        anchors {
            top: parent.top
            margins: 1.5 * mm
        }
        user: entry ? entry.author : null
        popBody: truncEntry.popBody
        visible: entry && entry.type != TlogEntry.AnonymousEntry
        acceptClick: !popBody || popBody.x <= 0
        onClicked: {
            avatarClicked();

            if (entry.tlog.id === entry.author.id)
                window.pushProfile(entry.tlog);
            else
                window.pushProfileById(entry.author.id);
        }
    }
    Loader {
        id: pin
        anchors {
            top: parent.top
            right: parent.right
            margins: 1.5 * mm
        }
        height: 20 * sp
        asynchronous: false
        Component.onDestruction: sourceComponent = undefined
        sourceComponent: Image {
            fillMode: Image.PreserveAspectFit
            source: (window.darkTheme ? '../icons/pin-white-'
                                      : '../icons/pin-black-')
                    + '128.png'
        }
    }
    ThemedText {
        id: nick
        anchors {
            top: parent.top
            left: entryAvatar.right
            right: pinVisible ? pin.left : parent.right
        }
        text: entry ? entry.author.name : ''
        font.pixelSize: window.fontSmaller
        elide: Text.ElideRight
        wrapMode: Text.NoWrap
        horizontalAlignment: Text.AlignLeft
        visible: entry && entry.type != TlogEntry.AnonymousEntry
    }
    ThemedText {
        id: date
        anchors {
            top: nick.visible ? nick.bottom : parent.top
            left: entryAvatar.visible ? entryAvatar.right : parent.left
            right: pinVisible ? pin.left : parent.right
        }
        text: entry ? entry.createdAt : ''
        color: window.secondaryTextColor
        font.pixelSize: window.fontSmallest
        elide: Text.AlignRight
        wrapMode: Text.NoWrap
    }
    Loader {
        id: firstImage
        anchors {
            top: entryAvatar.visible ? entryAvatar.bottom : date.bottom
            left: parent.left
            right: parent.right
            topMargin: 1.5 * mm
            bottomMargin: 1.5 * mm
        }
        readonly property AttachedImage image: entry ? entry.preview : null
        active: image && entry.type === TlogEntry.ImageEntry
        height: active ? (image.height / image.width * width) : 0
        asynchronous: false
        Component.onDestruction: sourceComponent = undefined
        sourceComponent: MyImage {
            url: firstImage.image ? firstImage.image.url : ''
            extension: firstImage.image ? firstImage.image.type : ''
            savable: true
            popBody: truncEntry.popBody
            acceptClick: false
            ThemedText {
                anchors {
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                    margins: 0.5 * mm
                }
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: window.fontSmaller
                style: Text.Outline
                styleColor: window.backgroundColor
                property int total: entry ? entry.attachedImagesModel.rowCount() : 0
                text: Tasty.num2str(total, 'изображение', 'изображения', 'изображений')
                visible: total > 1
                z: firstImage.z + 1
            }
        }
    }
    Loader {
        id: mediaLink
        anchors {
            top: firstImage.bottom
            left: parent.left
            right: parent.right
            bottomMargin: 1.5 * mm
        }
        active: entry && entry.media
        asynchronous: false
        Component.onDestruction: sourceComponent = undefined
        sourceComponent: MediaLink {
            media: entry ? entry.media : null
            acceptClick: false
            popBody: truncEntry.popBody
        }
    }
    ThemedText {
        id: entryTitle
        text: entry ? entry.truncatedTitle : ''
        anchors {
            top: mediaLink.bottom
            left: parent.left
            right: parent.right
        }
        font.pixelSize: entry && entry.truncatedText.length > 0 ? window.fontBigger
                                                                : window.fontNormal
        textFormat: Text.RichText
        visible: entry && entry.type != TlogEntry.QuoteEntry
        height: entry ? (visible && entry.truncatedTitle.length > 0
                         ? contentHeight : entry.truncatedText.length > 0 ? -2 * mm : 0) : 0
    }
    ThemedText {
        id: content
        text: entry ? entry.truncatedText : ''
        anchors {
            top: entryTitle.bottom
            left: parent.left
            right: parent.right
            leftMargin: entry && entry.type == TlogEntry.QuoteEntry ? 5 * mm : 1.5 * mm
            rightMargin: anchors.leftMargin
        }
        textFormat: Text.RichText
        height: entry ? (entry.truncatedText.length > 0 ? contentHeight
                                               : entry.truncatedTitle.length > 0 ? -2 * mm : 0) : 0
    }
    ThemedText {
        id: quoteSource
        anchors {
            top: content.bottom
            left: parent.left
            right: parent.right
        }
        visible: entry && entry.source.length > 0
        text: entry ? entry.source : ''
        font.pixelSize: window.fontSmaller
        font.italic: true
        textFormat: Text.RichText
        horizontalAlignment: Text.AlignRight
        height: visible ? contentHeight : -1.5 * mm
    }
    Rectangle {
        id: wc
        anchors {
            top: quoteSource.bottom
            left: parent.left
            margins: 1.5 * mm
        }
        height: 0.5 * mm
        radius: height / 2
        readonly property int maxWidth: parent.width - 3 * mm
        readonly property int length: entry ? Math.sqrt(entry.wordCount) / 32 * maxWidth : 0
        width: Math.min(length, maxWidth)
        color: window.secondaryTextColor
    }
}
