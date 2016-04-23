import QtQuick 2.3
import org.binque.taaasty 1.0

Rectangle {
    id: back
    color: window.backgroundColor
    property TlogEntry entry
    signal popped
    property bool poppable
MyListView {
    id: fullEntry
    anchors.fill: parent
//    property int entryId
//    property string title: ''
//    property string content: ''
//    property string date: ''
//    property string type: ''
//    property string commentAuthor: ''
//    property bool watching: false
//    property bool voted: false
//    property int votes: 0
//    property bool voteable: false
//    property bool canVote: false
//    property bool canWatch: false
//    property bool canFavorite: false
//    property bool favorited: false
//    property string url: ''
//    property int commentsCount: 0
//    property double wholeHeight: 0
    onPopped: back.popped()
    Component.onCompleted: {
        commentsModel.loadMore()
    }

    model: CommentsModel {
        id: commentsModel
        entryId: entry.id
    }

    delegate: Item {
        anchors.left: parent.left
        anchors.right: parent.right
        //color: window.backgroundColor
        height: 40 + commentText.paintedHeight + nameText.paintedHeight
        SmallAvatar {
            id: commentAvatar
            anchors.margins: 10
            width: 64
            height: 64
            source: !symbol ? thumb64 : ''
            name: userName
            symbol: !thumb64
        }
        Text {
            id: nameText
            text: userName
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
            text: createdAt //Ctrl.parseDate(updated_at)
            color: window.secondaryTextColor
            anchors.baseline: nameText.baseline
            anchors.right: parent.right
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            font.pointSize: 15
        }
        Text {
            id: commentText
            text: commentHtml
            color: window.textColor
            anchors.rightMargin: 10
            anchors.top: nameText.bottom
            anchors.left: nameText.left
            anchors.right: parent.right
            wrapMode: Text.Wrap
            font.pointSize: 17
            textFormat: Text.RichText
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                fullEntry.commentAuthor = userName;
                //window.showCommentMenu = true;
            }
        }
    }
    header: Item {
        id: fullEntryContent
        anchors.left: parent.left
        anchors.right: parent.right
        height: fullTitle.height + fullContent.height + 100
                + fullEntryFavButton.height + fullEntryDate.height
                + loadMoreButton.height
        // + fullEntryImages.height
//        ImagesView {
//            id: fullEntryImages
//            model: fullEntryAttach
//            height: fullEntry.wholeHeight * width
//            anchors.top: parent.top
//        }
        Text {
            id: fullTitle
            text: entry.title
//            anchors.top: fullEntryImages.bottom
            anchors.top: parent.top
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
//            visible: entry.canFavorite
//            enabled: !fullEntry.favorited
//            onClicked: Ctrl.favorites(fullEntry.entryId)
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
//            visible: fullEntry.canWatch // http delete?
//            enabled: !fullEntry.watching
//            onClicked: Ctrl.watchEntry(fullEntry.entryId, fullEntry.watching)
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
//            visible: fullEntry.voteable
//            enabled: fullEntry.canVote && !fullEntry.voted // not working on the site?
            onClicked: entry.rating.vote()
        }
        ThemedButton {
            id: loadMoreButton
            anchors.top: fullEntryVoteButton.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.margins: 10
            anchors.topMargin: 20
            text: 'Еще'
            height: 64
            width: parent.width / 3
            fontSize: fullEntryFavButton.fontSize
            visible: commentsModel.hasMore
            onClicked: commentsModel.loadMore()
        }
    }
//    footer: MessageEditor {
//        id: commentEditor
//        onSent: {
//            Ctrl.addComment(commentEditor.message);
//        }
//    }
    Poppable {
        body: back
    }
}
}
