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
    property int mode: Tasty.isAuthorized ? FeedModel.FriendsMode : FeedModel.LiveMode
    readonly property bool bestMode: back.mode === FeedModel.BestMode
                                     || back.mode === FeedModel.ExcellentMode
                                     || back.mode === FeedModel.WellMode
                                     || back.mode === FeedModel.GoodMode
                                     || back.mode === FeedModel.BetterThanMode
    property int tlogId: 0
    property int sinceId: 0
    property string sinceDate: ''
    property string slug: ''
    property int minRating: 0
    property string query: ''
    property string tag: ''
    property Tlog tlog: feedModel.tlog
    readonly property bool isFeedView: true
    function setMode(m, t, s) {
        query = '';

        if (t && t > 0)
            tlogId = t;
        else
            tlogId = 0;

        if (s)
            slug = s;
        else
            slug = '';

        if (m === FeedModel.TlogMode) {
            mode = m;
        }
        else if (mode === m) {
            feedModel.reset(m, t, s);
        }
        else {
            mode = m;
        }
    }
    Poppable {
        body: back
    }
    Splash {
        model: feedModel
        visible: !listView.visible
        emptyString: 'Нет записей'
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
        Component.onCompleted: {
            if (!back.tlogId)
                return;

            if (back.sinceId)
                feedModel.setSinceEntryId(back.sinceId);
            else if (back.sinceDate)
                feedModel.setSinceDate(back.sinceDate);
        }
        model: FeedModel {
            id: feedModel
            mode: back.mode
            tlogId: back.tlogId
            slug: back.slug
            minRating: back.minRating
            query: back.query
            tag: back.tag
        }
        delegate: Item {
            id: entryView
            width: window.width
            height: 7 * mm + entryVoteButton.y + entryVoteButton.height
            Poppable {
                body: back
                onClicked: {
                    if (back.x > 0) {
                        mouse.accepted = false;
                        return;
                    }

                    mouse.accepted = true;
                    window.pushFullEntry(entry, false, false, feedModel);
                }
            }
            ThemedText {
                id: repostText
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                    leftMargin: 5 * mm
                    rightMargin: anchors.leftMargin
                }
                color: window.secondaryTextColor
                font.pointSize: window.fontSmaller
                visible: entry.author.id !== entry.tlog.tlogId
                         && entry.tlog.tlogId !== tlog.tlogId
                height: visible ? contentHeight : - 1.5 * mm
                horizontalAlignment: Text.AlignHCenter
                text: entry.tlog.author.name
                      + (entry.tlog.author.title ? '\n' + entry.tlog.author.title : '')
                Poppable {
                    body: back
                    onClicked: {
                        if (back.x > 0) {
                            mouse.accepted = false;
                            return;
                        }

                        mouse.accepted = true;
                        window.pushTlog(entry.tlog.author.id);
                    }
                }
            }
            TruncatedEntry {
                id: truncEntry
                anchors {
                    top: repostText.bottom
                    left: parent.left
                    right: parent.right
                }
                entry: model.entry
                popBody: back
                pinVisible: model.entry.isFixed && feedModel.showFixed
            }
            IconButton {
                id: commentsButton
                anchors {
                    top: truncEntry.bottom
                    left: parent.left
                }
                visible: text
                text: entry.commentsCount || ''
                icon: (window.darkTheme ? '../icons/comment-light-'
                                        : '../icons/comment-dark-')
                      + '128.png'
                onClicked: {
                    if (back.x > 0)
                        backAnimation.start();
                    else
                        window.pushFullEntry(entry, false, true);
                }
            }
            IconButton {
                id: entryVoteButton
                anchors {
                    top: truncEntry.bottom
                    right: parent.right
                }
                readonly property bool votable: entry.rating.isVotable && Tasty.isAuthorized
                icon: (((votable === entry.rating.isVoted)
                       && (entry.rating.isBayesVoted || entry.rating.isVotedAgainst)
                       && (votable || entry.rating.isBayesVoted))
                       ? '../icons/flame-solid-' : '../icons/flame-outline-')
                      + '72.png'
                enabled: !entry.rating.isVotedAgainst || entry.rating.isVotable
                onClicked: {
                    entry.rating.vote();
                }
            }
            IconButton {
                id: entryVoteAgainstButton
                anchors {
                    top: truncEntry.bottom
                    right: entryRating.left
                }
                icon: (entry.rating.isVotedAgainst ? '../icons/drop-solid-'
                                                   : '../icons/drop-outline-')
                      + '72.png'
                enabled: !entry.rating.isBayesVoted
                onClicked: {
                    entry.rating.voteAgainst();
                }
            }
            ThemedText {
                id: entryRating
                anchors {
                    top: truncEntry.bottom
                    right: entryVoteButton.left
                }
                text: entry.isVotable ? '+ ' + entry.rating.votes : ''
                height: entryVoteButton.height
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                visible: entry.isVotable
            }
            Rectangle {
                id: br
                anchors {
                    left: entryVoteAgainstButton.right
                    right: entryVoteButton.left
                    bottom: entry.isVotable ? entryVoteButton.bottom : undefined
                    verticalCenter: entry.isVotable ? undefined : entryVoteButton.verticalCenter
                    margins: entry.isVotable ? 0 : 0.7 * mm
                }
                height: entry.isVotable ? 0.5 * mm : width
                radius: height / 2
                color: entry.rating.bayesRating > 5 ? '#FF5722' : entry.rating.bayesRating < -5
                                                    ? '#03A9F4' : window.secondaryTextColor
                Behavior on color {
                    ColorAnimation {
                        duration: 300
                    }
                }
            }
        }
        footer: ListBusyIndicator {
            running: feedModel.loading
            visible: feedModel.hasMore
        }
    }
}
