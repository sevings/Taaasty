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
    innerFlick: listView
    hasMenu: true
    property int mode: Tasty.isAuthorized ? FlowsModel.MyMode : FlowsModel.PopularMode
    readonly property bool isFlowsView: true
    Poppable {
        body: back
    }
    Splash {
        model: flowsModel
        visible: !listView.visible
        emptyString: 'Нет потоков'
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
        model: FlowsModel {
            id: flowsModel
            mode: back.mode
        }
        delegate: Item {
            width: window.width
            height: 7 * mm + flowPosts.y + flowPosts.height
            Poppable {
                body: back
                onClicked: {
                    if (back.x > 0) {
                        mouse.accepted = false;
                        return;
                    }

                    mouse.accepted = true;
                    window.pushTlog(flow.id);
                }
            }
            MyImage {
                id: flowPicture
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                    bottomMargin: 1.5 * mm
                }
                url: flow.picUrl
                width: window.width
                height: width * 9 / 16
                clip: true
                popBody: back
                acceptClick: false
            }
            ThemedText {
                id: flowName
                text: flow.name
                anchors {
                    top: flowPicture.bottom
                    left: parent.left
                    right: parent.right
                }
            }
            ThemedText {
                id: flowTitle
                text: flow.title
                font.pixelSize: window.fontSmaller
                anchors {
                    top: flowName.bottom
                    left: parent.left
                    right: parent.right
                }
                height: text.length === 0 ? -1.5 * mm : paintedHeight
            }
            ThemedText {
                id: flowFollowers
                text: flow.followersCount
                color: window.secondaryTextColor
                font.pixelSize: window.fontSmallest
                anchors {
                    top: flowTitle.bottom
                    right: parent.right
                }
            }
            ThemedText {
                id: flowPosts
                text: flow.entriesCount
                color: window.secondaryTextColor
                font.pixelSize: window.fontSmallest
                anchors {
                    top: flowTitle.bottom
                    left: parent.left
                }
            }
        }
        footer: ListBusyIndicator {
            running: flowsModel.loading
            visible: flowsModel.hasMore
        }
    }
}
