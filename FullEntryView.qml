import QtQuick 2.3
import org.binque.taaasty 1.0

Rectangle {
    id: back
    color: window.backgroundColor
    property TlogEntry entry
    property CommentsModel commentsModel: entry.commentsModel()
    property bool showProfiles: true
    signal avatarClicked(int tlogId)
    signal popped
    property bool poppable
    Poppable {
        body: back
    }
    MyListView {
        id: fullEntry
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: contentHeight > parent.height ? parent.height : contentHeight
        onPopped: back.popped()
        Component.onCompleted: {
            if (commentsModel.rowCount() === 0)
                commentsModel.loadMore();
        }
        model: commentsModel
        delegate: Item {
            anchors.left: parent.left
            anchors.right: parent.right
            //color: window.backgroundColor
            height: 40 + commentText.paintedHeight + nameText.paintedHeight
            Poppable {
                body: back
            }
            SmallAvatar {
                id: commentAvatar
                anchors.margins: 10
                width: 64
                height: 64
                source: comment.user.thumb64
                symbol: comment.user.symbol
                MouseArea {
                    anchors.fill: parent
                    onClicked: back.avatarClicked(comment.user.id);
                    enabled: showProfiles
                }
            }
            Text {
                id: nameText
                text: comment.user.name
                color: window.textColor
                anchors.top: parent.top
                anchors.left: commentAvatar.right
                anchors.right: commentDate.left
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                wrapMode: Text.Wrap
                font.pointSize: 20
                font.bold: true
                style: Text.Raised
                styleColor: window.brightColor
            }
            Text {
                id: commentDate
                text: comment.createdAt //Ctrl.parseDate(updated_at)
                color: window.secondaryTextColor
                anchors.baseline: nameText.baseline
                anchors.right: parent.right
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                font.pointSize: 15
            }
            Text {
                id: commentText
                text: comment.html
                color: window.textColor
                anchors.rightMargin: 10
                anchors.top: nameText.bottom
                anchors.left: nameText.left
                anchors.right: parent.right
                wrapMode: Text.Wrap
                font.pointSize: 17
                textFormat: Text.RichText
            }

        }
        header: Item {
            id: fullEntryContent
            anchors.left: parent.left
            anchors.right: parent.right
            height: fullTitle.height + fullContent.height + 100
                    + fullEntryFavButton.height + fullEntryDate.height
                    + loadMoreButton.height
                    + fullEntryImages.height
            Poppable {
                body: back
            }
            ListView {
                id: fullEntryImages
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.topMargin: 10
                anchors.bottomMargin: 10
                interactive: false
                //spacing: 10
                property AttachedImagesModel imagesModel: entry.attachedImagesModel()
                height: imagesModel.listRatio() * width
                        + (imagesModel.rowCount() - 1) * 10
                model: imagesModel
                delegate: MyImage {
                    id: picture
                    anchors.topMargin: 10
                    anchors.bottomMargin: 10
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: image.height / image.width * width
                    source: image.url
                    type: image.type
                }
            }
            Text {
                id: fullTitle
                text: entry.title
                anchors.top: fullEntryImages.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 10
                wrapMode: Text.Wrap
                font.pointSize: entry.text.length > 0 ? 25 : 20
                color: window.textColor
                textFormat: Text.RichText
                height: entry.title.length > 0 ? paintedHeight : entry.text.length > 0 ? -20 : 0
            }
            Text {
                id: fullContent
                text: entry.text
                anchors.top: fullTitle.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 10
                wrapMode: Text.Wrap
                font.pointSize: 20
                color: window.textColor
                textFormat: Text.RichText
                height: entry.text.length > 0 ? paintedHeight : entry.title.length > 0 ? -20 : 0
            }
            Text {
                id: fullEntryDate
                text: entry.createdAt
                color: window.secondaryTextColor
                anchors.top: fullContent.bottom
                anchors.left: parent.left
                anchors.margins: 10
                font.pointSize: 15
            }
            Text {
                id: fullEntryCommentsCount
                text: entry.commentsCount + ' коммент.'
                color: window.secondaryTextColor
                anchors.top: fullContent.bottom
                anchors.right: parent.right
                anchors.margins: 10
                font.pointSize: 15
            }
            ThemedButton {
                id: fullEntryFavButton
                anchors.top: fullEntryDate.bottom
                anchors.left: parent.left
                anchors.margins: 10
                anchors.topMargin: 20
                text: '*'
                height: 64
                width: (parent.width - 40) / 3
                fontSize: 20
                visible: entry.isFavoritable
                checked: entry.isFavorited
//            enabled: !fullEntry.favorited
                onClicked: entry.favorite()
            }
            ThemedButton {
                id: fullEntryWatchButton
                anchors.top: fullEntryDate.bottom
                anchors.left: fullEntryFavButton.right
                anchors.margins: 10
                anchors.topMargin: 20
                text: 'V'
                height: 64
                width: fullEntryFavButton.width
                fontSize: fullEntryFavButton.fontSize
                visible: entry.isWatchable
                checked: entry.isWatched
//              enabled: entry.isWatchable
                onClicked: entry.watch()
            }
            ThemedButton {
                id: fullEntryVoteButton
                anchors.top: fullEntryDate.bottom
                anchors.left: fullEntryWatchButton.right
                anchors.right: parent.right
                anchors.margins: 10
                anchors.topMargin: 20
                text: '+ ' + entry.rating.votes
                height: 64
                fontSize: fullEntryFavButton.fontSize
                visible: entry.isVotable
                enabled: entry.rating.isVotable
                checked: entry.rating.isVoted
                onClicked: entry.rating.vote()
            }
            ThemedButton {
                id: loadMoreButton
                anchors.top: fullEntryVoteButton.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.margins: 10
                anchors.topMargin: 20
                text: 'Еще'
                height: visible ? 64 : 0
                width: parent.width / 3
                fontSize: fullEntryFavButton.fontSize
                visible: commentsModel.hasMore && !commentsModel.loading
                onClicked: commentsModel.loadMore()
            }
        }
        footer: MessageEditor {
            id: commentEditor
            onSent: {
                entry.addComment(commentEditor.message);
            }
        }
    }
}
