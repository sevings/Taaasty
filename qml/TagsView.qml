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
import QtQuick.Controls.Material 2.1
import org.binque.taaasty 1.0

Pane {
    id: back
    property int tlogId: 0
    property Tlog tlog: Tlog {
        tlogId: back.tlogId
    }
    Poppable {
        body: back
    }
    Splash {
        id: splash
        model: tagsModel
        visible: !listView.visible
        emptyString: 'Нет тегов'
    }
    TagsModel {
        id: tagsModel
        Component.onCompleted: {
            setTlog(back.tlogId);
        }
    }
    MyListView {
        id: listView
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: contentHeight > parent.height ? parent.height : contentHeight
        visible: count > 0
        model: tagsModel
        delegate: Rectangle {
            id: tagView
            width: window.width
            height: 5 * mm + tagName.height
            color: pop.pressed ? Material.primary : 'transparent'
            Poppable {
                id: pop
                body: back
                onClicked: {
                    if (back.x > 0) {
                        mouse.accepted = false;
                        return;
                    }

                    window.pushTlogTag(back.tlog, model.tagName);
                }
            }
            ThemedText {
                id: tagName
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    right: tagCount.left
                    margins: 1.5 * mm
                }
                text: '#' + model.tagName
                elide: Text.ElideRight
                wrapMode: Text.NoWrap
                font.pixelSize: window.fontBigger
                font.italic: true
            }
            ThemedText {
                id: tagCount
                anchors {
                    verticalCenter: parent.verticalCenter
                    right: parent.right
                    margins: 1.5 * mm
                }
                text: model.tagCount
                color: window.secondaryTextColor
                font.pixelSize: window.fontBigger
            }
        }
    }
}
