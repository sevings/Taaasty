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
import org.binque.taaasty 1.0

Pane {
    id: back
    innerFlick: fullEntry
    property int entryId
    property TlogEntry entry: TlogEntry {
        entryId: back.entryId
    }
    readonly property Tlog tlog: entry.tlog
    property CommentsModel commentsModel: entry.comments
    property bool showProfiles: tlog.tlogId !== window.anonymousId
    property bool scrollToBottom: false
    property bool scrollToFirst: false
    readonly property bool isFullEntryView: true
    signal addGreeting(string slug)
    function checkComments() {
        if (fullEntry.count > 0 || !commentsModel.hasMore)
            commentsModel.check();
        else
            commentsModel.loadMore();
    }
    function scrollWhereNeed() {
        if (!scrollToBottom && !scrollToFirst)
            return;

        if (fullEntry.count <= 0) {
            //fullEntry.positionViewAtEnd();
            fullEntry.contentY = fullEntry.contentHeight;
            fullEntry.returnToBounds();
            console.log(fullEntry.contentY)
        }
        else if (scrollToBottom)
            fullEntry.positionViewAtEnd();
        else if (scrollToFirst)
            fullEntry.positionViewAtIndex(0, ListView.Beginning);
    }
    Connections {
        target: entry
        onUpdated: {
            checkComments();
            scrollWhereNeed();
        }
        onEntryDeleted: {
            window.popFromStack();
        }
        onCommentAdded: {
            fullEntry.positionViewAtEnd();
        }
    }
    Component.onCompleted: {
        checkComments();
        if (!entry.createdAt.length)
            entry.reload();
        else
            scrollWhereNeed();
    }
    onVisibleChanged: {
        if (visible)
            checkComments();
    }
    Timer {
        interval: 20000
        running: back.visible && (!entry.chat || !entry.chat.isInvolved)
        repeat: true
        onTriggered: commentsModel.check()
    }
    Poppable {
        body: back
    }
    Splash {
        visible: !fullEntry.visible
        running: entry.loading
        text: entry.errorString
    }
    MyListView {
        id: fullEntry
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: contentHeight > parent.height ? parent.height : contentHeight
        visible: !entry.loading && !entry.errorString
        model: commentsModel
        delegate: Item {
            width: window.width
            readonly property int textHeight: nameText.height + commentText.height - 1.5 * mm
            height: (textHeight > commentAvatar.height ? textHeight : commentAvatar.height) + 4 * mm
//            color: pop.pressed ? Material.primary : 'transparent'
            Component.onCompleted: {
                if (entry.chat)
                    entry.chat.readAll();
            }
            Poppable {
                id: pop
                body: back
                onClicked: {
                    menu.show(comment);
                }
            }
            SmallAvatar {
                id: commentAvatar
                anchors {
                    margins: 1.5 * mm
                    topMargin: 2 * mm
                }
                user: comment.user
                popBody: back
                onClicked: {
                    if (back.showProfiles)
                        window.pushProfileById(comment.user.id);
                }
            }
            Q.Label {
                id: nameText
                text: comment.user.name
                anchors {
                    top: parent.top
                    left: commentAvatar.right
                    right: commentDate.left
                    leftMargin: 1.5 * mm
                    rightMargin: 1.5 * mm
                    topMargin: 2 * mm
                }
                wrapMode: Text.Wrap
                font.pointSize: window.fontSmaller
                font.bold: true
                style: Text.Raised
                styleColor: Material.accent
            }
            Q.Label {
                id: commentDate
                text: comment.createdAt
                color: window.secondaryTextColor
                anchors {
                    baseline: nameText.baseline
                    right: parent.right
                    leftMargin: 1.5 * mm
                    rightMargin: 1.5 * mm
                }
                font.pointSize: window.fontSmaller
            }
            Q.Label {
                id: commentText
                text: comment.text
                anchors {
                    rightMargin: 1.5 * mm
                    top: nameText.bottom
                    left: nameText.left
                    right: parent.right
                }
                wrapMode: Text.Wrap
                font.pointSize: window.fontSmaller
                textFormat: Text.RichText
                onLinkActivated: window.openLink(link)
            }
        }
        header: Item {
            id: fullEntryContent
            width: window.width
            height: 14 * mm + fullTitle.height + fullContent.height + quoteSource.height
                    + fullEntryVoteButton.height + fullEntryDate.height
                    + (busy.running ? busy.height : loadMoreButton.height)
                    + fullEntryImages.height + mediaLink.height
            Poppable {
                body: back
            }
            ListView {
                id: fullEntryImages
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                    bottomMargin: 1.5 * mm
                }
                interactive: false
                spacing: 1.5 * mm
                property AttachedImagesModel imagesModel: entry.attachedImagesModel
                height: imagesModel ? (imagesModel.listRatio() * window.width
                        + (imagesModel.rowCount() - 1) * mm) : 0
                model: imagesModel
                delegate: MyImage {
                    id: picture
                    width: window.width
                    height: image.height / image.width * width
                    url: image.url
                    extension: image.type
                    savable: true
                    popBody: back
                }
            }
            MediaLink {
                id: mediaLink
                visible: entry.media
                anchors {
                    top: fullEntryImages.bottom
                    left: parent.left
                    right: parent.right
                    bottomMargin: 1.5 * mm
                }
                media: entry.media
                acceptClick: mediaLink.url
                popBody: back
                onClicked: {
                    mediaLink.play();
                }
            }
            ThemedText {
                id: fullTitle
                text: entry.title
                anchors {
                    top: mediaLink.bottom
                    left: parent.left
                    right: parent.right
                }
                font.pointSize: (entry.text.length > 0 ? window.fontBigger
                                                      : window.fontNormal)
                textFormat: Text.RichText
                height: entry.title.length > 0 ? contentHeight : entry.text.length > 0 ? -1.5 * mm : 0
                onLinkActivated: window.openLink(link)
            }
            ThemedText {
                id: fullContent
                text: entry.text
                anchors {
                    top: fullTitle.bottom
                    left: parent.left
                    right: parent.right
                    leftMargin: entry.type === 'quote' ? 5 * mm : 1.5 * mm
                    rightMargin: anchors.leftMargin
                }
                textFormat: Text.RichText
                height: entry.text.length > 0 ? contentHeight : entry.title.length > 0 ? -1.5 * mm : 0
                onLinkActivated: window.openLink(link)
            }
            ThemedText {
                id: quoteSource
                text: entry.source
                anchors {
                    top: fullContent.bottom
                    left: parent.left
                    right: parent.right
                }
                font.pointSize: window.fontSmaller
                font.italic: true
                textFormat: Text.RichText
                height: entry.source.length > 0 ? contentHeight : -1.5 * mm
                horizontalAlignment: Text.AlignRight
            }
            Text {
                id: fullEntryDate
                anchors {
                    top: quoteSource.bottom
                    left: parent.left
                    margins: 1.5 * mm
                }
                text: entry.createdAt
                color: window.secondaryTextColor
                font.pointSize: window.fontSmallest
            }
            IconButton {
                id: commentsButton
                anchors {
                    top: fullEntryDate.bottom
                    left: parent.left
                }
//                enabled: false
                text: entry.commentsCount || ''
                icon: (window.darkTheme ? '../icons/comment-light-'
                                        : '../icons/comment-dark-')
                      + '128.png'
            }
            IconButton {
                id: fullEntryVoteButton
                anchors {
                    top: fullEntryDate.bottom
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
                id: fullEntryVoteAgainstButton
                anchors {
                    top: fullEntryDate.bottom
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
                    top: fullEntryDate.bottom
                    right: fullEntryVoteButton.left
                }
                text: entry.isVotable ? '+ ' + entry.rating.votes : ''
                height: fullEntryVoteButton.height
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                visible: entry.isVotable
            }
            Rectangle {
                id: br
                anchors {
                    left: fullEntryVoteAgainstButton.right
                    right: fullEntryVoteButton.left
                    bottom: entry.isVotable ? fullEntryVoteButton.bottom : undefined
                    verticalCenter: entry.isVotable ? undefined : fullEntryVoteButton.verticalCenter
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
            ThemedButton {
                id: loadMoreButton
                anchors {
                    top: fullEntryVoteButton.bottom
                    horizontalCenter: parent.horizontalCenter
                    margins: 1.5 * mm
                    topMargin: 2 * mm
                }
                text: enabled || fullEntry.count > 0 ? 'Еще' : ''
                height: visible ? implicitHeight : 0 // changing height forces layout
                width: 20 * mm
                visible: commentsModel && commentsModel.hasMore && !commentsModel.loading
                highlighted: true
                onClicked: commentsModel.loadMore()
            }
            Q.BusyIndicator {
                id: busy
                anchors {
                    top: fullEntryVoteButton.bottom
                    horizontalCenter: parent.horizontalCenter
                    margins: 1.5 * mm
                    topMargin: 2 * mm
                }
                running: commentsModel && commentsModel.loading
                visible: commentsModel && commentsModel.hasMore
            }
        }
        footer: MessageEditor {
            id: commentEditor
            popBody: back
            visible: Tasty.isAuthorized
            height: visible ? implicitHeight : 1.5 * mm
            z: fullEntry.count + 10
            onSent: {
                entry.addComment(commentEditor.message);
            }
            onActiveFocusChanged: {
                if (focus)
                    fullEntry.positionViewAtEnd();
            }
            Connections {
                target: entry
                onCommentAdded: {
                    commentEditor.clear();
                }
                onAddingCommentError: {
                    commentEditor.uploading = false;
                }
            }
            Connections {
                target: back
                onAddGreeting: {
                    commentEditor.addGreeting(slug);
                }
                onHeightChanged: { // keyboard shown
                    if (commentEditor.focus)
                        fullEntry.positionViewAtEnd();
                }
            }
            Connections {
                target: fullEntry
                onMovingVerticallyChanged: commentEditor.hideMenu()
            }
            Connections {
                target: menu
                onOpened: commentEditor.hideMenu()
            }
        }
    }
    Popup {
        id: menu
        height: menuColumn.height + 2 * mm
        anchors.margins: 2 * mm
        property Comment comment: Comment { }
        function close() {
            state = "closed";
        }
        function show(cmt) {
            menu.comment = cmt;
            window.hideFooter();
            state = "opened";
        }
        onClosing: menu.close()
        Column {
            id: menuColumn
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                topMargin: 1.5 * mm
                bottomMargin: 1.5 * mm
            }
            spacing: 1.5 * mm
            MenuItem {
                text: 'Ответить'
                onTriggered: {
                    addGreeting(menu.comment.user.slug);
                    menu.close();
                    fullEntry.positionViewAtEnd();
                }
            }
//            ThemedButton {
//                anchors.left: parent.left
//                anchors.right: parent.right
//                text: 'Править'
//                onClicked: {
//    //                    addGreeting(menu.comment.user.slug);
//                    menu.close();
//                }
//                visible: menu.comment && menu.comment.isEditable === true
//            }
//            ThemedButton {
//                anchors {
//                    left: parent.left
//                    right: parent.right
//                }
//                text: 'Удалить'
//                onClicked: {
//                    menu.comment.remove();
//                    menu.close();
//                }
//                visible: menu.comment && menu.comment.isDeletable === true
//            }
        }
    }
}
