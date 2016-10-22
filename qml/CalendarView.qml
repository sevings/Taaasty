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

Pane {
    id: back
    readonly property int tlogId: tlog.tlogId
    property Tlog tlog: Tlog {

    }
    Poppable {
        body: back
    }
    Splash {
        id: splash
        visible: !listView.visible
        running: calendarModel.loading
        text: calendarModel.errorString || 'Нет записей'
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
//        interactive: back.x == 0
        model: CalendarModel {
            id: calendarModel
            Component.onCompleted: {
                setTlog(back.tlogId);
            }
        }
        section.property: 'entry.month'
        section.criteria: ViewSection.FullString
        section.labelPositioning: ViewSection.InlineLabels | ViewSection.CurrentLabelAtStart
        section.delegate: Rectangle {
            color: sectionMouse.pressed ? Material.accent : Material.primary
            width: window.width
            height: 5 * mm + monthText.contentHeight
            Poppable {
                id: sectionMouse
                body: back
                onClicked: {
                    mouse.accepted = true;
                    var e = calendarModel.firstMonthEntry(section);
                    if (back.tlog.author.isDaylog)
                        window.pushTlog(back.tlogId, 0, e.date);
                    else
                        window.pushTlog(back.tlogId, e.id);
                }
            }
            ThemedText {
                id: monthText
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    right: parent.right
                }
                horizontalAlignment: Text.AlignHCenter
                text: section
                font.pointSize: window.fontBigger
            }
        }
        delegate: Rectangle {
            id: entryView
            width: window.width
            height: 5 * mm + date.height + entryTitle.height + comments.height
            color: pop.pressed ? Material.primary : 'transparent'
            Poppable {
                id: pop
                body: back
                onClicked: {
                    if (back.x > 0) {
                        mouse.accepted = false;
                        return;
                    }

                    window.pushFullEntry(entry.fullEntry);
                }
            }
            ThemedText {
                id: entryTitle
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                    topMargin: 2 * mm
                }
                text: entry.truncatedTitle || '(без заголовка)'
                textFormat: Text.RichText
                horizontalAlignment: Text.AlignHCenter
            }
            ThemedText {
                id: date
                anchors {
                    top: entryTitle.bottom
                    left: parent.left
                    right: comments.left
                }
                text: entry.createdAt
                color: window.secondaryTextColor
                font.pointSize: window.fontSmallest
                elide: Text.AlignRight
                wrapMode: Text.NoWrap
            }
            ThemedText {
                id: comments
                anchors {
                    top: entryTitle.bottom
                    right: parent.right
                }
                text: entry.commentsCount + ' коммент.'
                font.pointSize: window.fontSmallest
                color: window.secondaryTextColor
            }
        }
    }
}
