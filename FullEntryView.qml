import QtQuick 2.3
import org.binque.taaasty 1.0

Rectangle {
    id: back
    color: window.backgroundColor
    property int entryId
    property TlogEntry entry: TlogEntry {
        entryId: back.entryId
    }
    readonly property Tlog tlog: entry.tlog
    property CommentsModel commentsModel: entry.commentsModel
    property bool showProfiles: true
    signal avatarClicked(int tlogId)
    signal popped
    property bool poppable
    readonly property bool isFullEntryView: true
    Component.onCompleted: {
        if (commentsModel.rowCount() === 0)
            commentsModel.loadMore();
        else
            commentsModel.check();
    }
    Poppable {
        body: back
    }
    Splash {
        visible: !fullEntry.visible
    }
    MyListView {
        id: fullEntry
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: contentHeight > parent.height ? parent.height : contentHeight
        visible: !entry.loading
        model: commentsModel
        delegate: Item {
//            anchors.left: parent.left
//            anchors.right: parent.right
            //color: window.backgroundColor
            width: window.width
            height: 4 * mm + commentText.paintedHeight + nameText.paintedHeight
            Poppable {
                body: back
            }
            SmallAvatar {
                id: commentAvatar
                anchors.margins: 1 * mm
                user: comment.user
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
                anchors.leftMargin: 1 * mm
                anchors.rightMargin: 1 * mm
                wrapMode: Text.Wrap
                font.pointSize: window.fontNormal
                font.bold: true
                style: Text.Raised
                styleColor: window.brightColor
            }
            Text {
                id: commentDate
                text: comment.createdAt
                color: window.secondaryTextColor
                anchors.baseline: nameText.baseline
                anchors.right: parent.right
                anchors.leftMargin: 1 * mm
                anchors.rightMargin: 1 * mm
                font.pointSize: window.fontSmaller
            }
            Text {
                id: commentText
                text: comment.html
                color: window.textColor
                anchors.rightMargin: 1 * mm
                anchors.top: nameText.bottom
                anchors.left: nameText.left
                anchors.right: parent.right
                wrapMode: Text.Wrap
                font.pointSize: window.fontSmaller
                textFormat: Text.RichText
                onLinkActivated: Qt.openUrlExternally(link)
            }
        }
        header: Item {
            id: fullEntryContent
            anchors.left: parent.left
            anchors.right: parent.right
            height: 9 * mm + fullTitle.height + fullContent.height + quoteSource.height
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
//                anchors.topMargin: 1 * mm
                anchors.bottomMargin: 1 * mm
                interactive: false
                //spacing: 10
                property AttachedImagesModel imagesModel: entry.attachedImagesModel
                height: imagesModel ? (imagesModel.listRatio() * window.width
                        + (imagesModel.rowCount() - 1) * 10) : 0
                model: imagesModel
                delegate: MyImage {
                    id: picture
                    anchors.topMargin: 1 * mm
                    anchors.bottomMargin: 1 * mm
//                    anchors.left: parent.left
//                    anchors.right: parent.right
                    width: window.width
                    height: image.height / image.width * width
                    url: image.url
                    extension: image.type
                }
            }
            Text {
                id: fullTitle
                text: entry.title
                anchors.top: fullEntryImages.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 1 * mm
                wrapMode: Text.Wrap
                font.pointSize: (entry.text.length > 0 ? window.fontBigger
                                                      : window.fontNormal)
                color: window.textColor
                textFormat: Text.RichText
                height: entry.title.length > 0 ? paintedHeight : entry.text.length > 0 ? -2 * mm : 0
                onLinkActivated: Qt.openUrlExternally(link)
            }
            Text {
                id: fullContent
                text: entry.text
                anchors.top: fullTitle.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 1 * mm
                anchors.leftMargin: entry.type === 'quote' ? 5 * mm : 1 * mm
                anchors.rightMargin: anchors.leftMargin
                wrapMode: Text.Wrap
                font.pointSize: window.fontNormal
                color: window.textColor
                textFormat: Text.RichText
                height: entry.text.length > 0 ? paintedHeight : entry.title.length > 0 ? -2 * mm : 0
                onLinkActivated: Qt.openUrlExternally(link)
            }
            Text {
                id: quoteSource
                text: entry.source
                anchors.top: fullContent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 1 * mm
                wrapMode: Text.Wrap
                font.pointSize: window.fontSmaller
                font.italic: true
                color: window.textColor
                textFormat: Text.RichText
                height: entry.source.length > 0 ? paintedHeight : -1 * mm
                horizontalAlignment: Text.AlignRight
            }
            Text {
                id: fullEntryDate
                text: entry.createdAt
                color: window.secondaryTextColor
                anchors.top: quoteSource.bottom
                anchors.left: parent.left
                anchors.margins: 1 * mm
                font.pointSize: window.fontSmallest
            }
            Text {
                id: fullEntryCommentsCount
                text: entry.commentsCount + ' коммент.'
                color: window.secondaryTextColor
                anchors.top: quoteSource.bottom
                anchors.right: parent.right
                anchors.margins: 1 * mm
                font.pointSize: window.fontSmallest
            }
            ThemedButton {
                id: fullEntryFavButton
                anchors.top: fullEntryDate.bottom
                anchors.left: parent.left
                anchors.margins: 1 * mm
                anchors.topMargin: 2 * mm
                text: '*'
                height: 6 * mm
                width: (parent.width - 4 * mm) / 3
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
                anchors.margins: 1 * mm
                anchors.topMargin: 2 * mm
                text: 'V'
                height: 6 * mm
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
                anchors.margins: 1 * mm
                anchors.topMargin: 2 * mm
                text: '+ ' + entry.rating.votes
                height: 6 * mm
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
                anchors.margins: 1 * mm
                anchors.topMargin: 2 * mm
                text: 'Еще'
                height: visible ? 6 * mm : 0
                width: parent.width / 3
                fontSize: fullEntryFavButton.fontSize
                visible: commentsModel && commentsModel.hasMore
                enabled: commentsModel && !commentsModel.loading
                onClicked: commentsModel.loadMore()
            }
        }
        footer: MessageEditor {
            id: commentEditor
            onSent: {
                entry.addComment(commentEditor.message);
            }
            Connections {
                target: entry
                onCommentAdded: {
                    commentEditor.clear();
                }
                onUpdated: {
                    if (commentsModel.rowCount() === 0)
                        commentsModel.loadMore();

                    window.setFooterTlog(entry.tlog);
                }
            }
        }
    }
}
