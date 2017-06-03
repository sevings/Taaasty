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
import QtQuick.Controls.Material 2.2
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
        active: !listView.visible
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
        visible: model.size > 0
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
            readonly property TlogEntry tlogEntry: model.entry
            Component.onCompleted: {
                if (index > feedModel.size - 50 && feedModel.hasMore)
                    feedModel.loadMore();
            }
            Poppable {
                body: back
                onClicked: {
                    if (back.x > 0) {
                        mouse.accepted = false;
                        return;
                    }

                    mouse.accepted = true;
                    window.pushFullEntry(tlogEntry, false, false, feedModel);
                }
            }
            Loader {
                id: repostText
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                    leftMargin: 5 * mm
                    rightMargin: anchors.leftMargin
                }
                active: tlogEntry && tlogEntry.author.id !== tlogEntry.tlog.tlogId
                        && tlogEntry.tlog.tlogId !== tlog.tlogId
                asynchronous: false
                Component.onDestruction: sourceComponent = undefined
                sourceComponent: ThemedText {
                    color: window.secondaryTextColor
                    font.pixelSize: window.fontSmaller
                    height: contentHeight
                    horizontalAlignment: Text.AlignHCenter
                    text: tlogEntry && tlogEntry.tlog.author.name
                          + (tlogEntry.tlog.author.title ? '\n' + tlogEntry.tlog.author.title : '')
                    Poppable {
                        body: back
                        onClicked: {
                            if (back.x > 0) {
                                mouse.accepted = false;
                                return;
                            }

                            mouse.accepted = true;
                            window.pushTlog(tlogEntry.tlog.author.id);
                        }
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
                entry: tlogEntry
                popBody: back
                pinVisible: tlogEntry && tlogEntry.isFixed && feedModel.showFixed
            }
            IconButton {
                id: commentsButton
                anchors {
                    top: truncEntry.bottom
                    left: parent.left
                }
                visible: text
                text: tlogEntry ? tlogEntry.commentsCount : ''
                icon: (window.darkTheme ? '../icons/comment-light-'
                                        : '../icons/comment-dark-')
                      + '128.png'
                onClicked: {
                    if (back.x > 0)
                        backAnimation.start();
                    else
                        window.pushFullEntry(tlogEntry, false, true);
                }
            }
            IconButton {
                id: entryVoteButton
                anchors {
                    top: truncEntry.bottom
                    right: parent.right
                }
                readonly property bool votable: tlogEntry && tlogEntry.rating.isVotable && Tasty.isAuthorized
                icon: (tlogEntry && ((votable === tlogEntry.rating.isVoted)
                       && (tlogEntry.rating.isBayesVoted || tlogEntry.rating.isVotedAgainst)
                       && (votable || tlogEntry.rating.isBayesVoted))
                       ? '../icons/flame-solid-' : '../icons/flame-outline-')
                      + '72.png'
                enabled: tlogEntry && (!tlogEntry.rating.isVotedAgainst || tlogEntry.rating.isVotable)
                onClicked: {
                    tlogEntry.rating.vote();
                }
            }
            IconButton {
                id: entryVoteAgainstButton
                anchors {
                    top: truncEntry.bottom
                    right: entryRating.left
                }
                icon: (tlogEntry && tlogEntry.rating.isVotedAgainst ? '../icons/drop-solid-'
                                                   : '../icons/drop-outline-')
                      + '72.png'
                enabled: tlogEntry && !tlogEntry.rating.isBayesVoted
                onClicked: {
                    tlogEntry.rating.voteAgainst();
                }
            }
            ThemedText {
                id: entryRating
                anchors {
                    top: truncEntry.bottom
                    right: entryVoteButton.left
                }
                text: tlogEntry && tlogEntry.isVotable ? '+ ' + tlogEntry.rating.votes : ''
                height: entryVoteButton.height
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                visible: tlogEntry && tlogEntry.isVotable
            }
            Rectangle {
                id: br
                anchors {
                    left: entryVoteAgainstButton.right
                    right: entryVoteButton.left
                    bottom:tlogEntry &&  tlogEntry.isVotable ? entryVoteButton.bottom : undefined
                    verticalCenter: tlogEntry && tlogEntry.isVotable ? undefined : entryVoteButton.verticalCenter
                    margins: tlogEntry && tlogEntry.isVotable ? 0 : 0.7 * mm
                }
                height: tlogEntry && tlogEntry.isVotable ? 0.5 * mm : width
                radius: height / 2
                color:tlogEntry &&  tlogEntry.rating.bayesRating > 5 ? '#FF5722'
                                                    : tlogEntry && tlogEntry.rating.bayesRating < -5
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
