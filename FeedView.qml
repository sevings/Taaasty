import QtQuick 2.3
import QtQuick.Controls 1.2
import org.binque.taaasty 1.0

Rectangle {
    id: back
    property int mode: FeedModel.LiveMode
    property int tlog
    signal entryClicked(TlogEntry entry)
    signal popped
    property bool poppable
    color: window.backgroundColor
    MyListView {
        anchors.fill: parent
        model: FeedModel {
            id: feedModel
            mode: back.mode
            tlog: back.tlog
        }
        delegate: Item {
            id: entryView
            anchors.left: parent.left
            anchors.right: parent.right
            property color fontColor: window.textColor
            height: 50 + content.height + entryTitle.height + entryAvatar.height + comments.height + firstImage.height
            Poppable {
                body: back
                onClicked: {
                    entryClicked(entry);
                }
            }
            SmallAvatar {
                id: entryAvatar
                anchors.margins: 10
                source: entry.author.userpic.thumb64_url || ''
                symbol: entry.author.userpic.symbol
            }
            Text {
                id: nick
                text: entry.author.name
                color: entryView.fontColor
                font.pointSize: 20
                anchors.top: parent.top
                anchors.left: entryAvatar.right
                anchors.right: entryVoteButton.left
                anchors.margins: 10
                elide: Text.AlignRight
                horizontalAlignment: Text.AlignLeft
            }
            Text {
                id: date
                text: entry.createdAt
                color: window.secondaryTextColor
                font.pointSize: 14
                anchors.top: nick.bottom
                anchors.left: entryAvatar.right
                anchors.margins: 10
            }
            ThemedButton {
                id: entryVoteButton
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.margins: 10
                height: 64
                width: parent.width / 5
                text: '+ ' + entry.rating.votes
                visible: entry.isVotable
                enabled: entry.rating.isVotable
                checked: entry.rating.isVoted
                fontSize: 20
                onClicked: {
                    entry.rating.vote();
                }
            }
            MyImage {
                id: firstImage
                property AttachedImage image: entry.attachedImagesModel().first()
                visible: image
                anchors.top: entryAvatar.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.topMargin: 10
                anchors.bottomMargin: 10
                height: visible ? (image.height / image.width * width) : 0
                source: visible ? image.url : ''
                type: visible ? image.type : ''
                Text {
                    anchors.bottom: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.margins: 5
                    font.pointSize: 14
                    color: window.textColor
                    style: Text.Outline
                    styleColor: window.backgroundColor
                    property int total: entry.attachedImagesModel().rowCount()
                    text: total + ' images'
                    visible: total > 1
                }
            }
            Text {
                id: entryTitle
                text: entry.truncatedTitle
                anchors.top: firstImage.bottom
//                anchors.top: entryAvatar.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 10
                wrapMode: Text.Wrap
                font.pointSize: entry.truncatedText.length > 0 ? 25 : 20
                color: parent.fontColor
                textFormat: Text.RichText
                height: entry.truncatedTitle.length > 0 ? paintedHeight
                                                        : entry.truncatedText.length > 0 ? -20 : 0
            }
            Text {
                id: content
                text: entry.truncatedText
                anchors.top: entryTitle.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 10
                wrapMode: Text.Wrap
                font.pointSize: 20
                color: parent.fontColor
                textFormat: Text.RichText
                height: entry.truncatedText.length > 0 ? paintedHeight
                                                       : entry.truncatedTitle.length > 0 ? -20 : 0
            }
            Text {
                id: comments
                text: entry.commentsCount + ' коммент.'
                color: entryView.fontColor
                font.pointSize: 15
                anchors.top: content.bottom
                anchors.right: parent.right
                anchors.margins: 10
            }
        }
    }
}
