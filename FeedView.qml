import QtQuick 2.5
import org.binque.taaasty 1.0

Rectangle {
    id: back
    color: window.backgroundColor
    signal popped
    property bool poppable
    property int mode: FeedModel.LiveMode
    property int tlogId: 0
    property Tlog tlog: Tlog {
        tlogId: back.tlogId
    }
    readonly property string title: { // for footer
        switch (back.mode) {
        case FeedModel.MyTlogMode:
            'My tlog'; break;
        case FeedModel.LiveMode:
            'Live'; break;
        case FeedModel.FriendsMode:
            'Friends'; break;
        case FeedModel.ExcellentMode:
            'Excellent'; break;
        case FeedModel.BestMode:
            'Best'; break;
        case FeedModel.GoodMode:
            'Good'; break;
        case FeedModel.WellMode:
            'Well'; break;
        case FeedModel.AnonymousMode:
            'Anonymous'; break;
        case FeedModel.FavoritesMode:
            'Favorites'; break;
        default:
            'Tlog'
        }
    }
    readonly property bool customTitle: mode !== FeedModel.TlogMode// && mode !== FeedModel.MyTlogMode
    readonly property bool isFeedView: true
    signal pushed
    signal entryClicked(TlogEntry entry)
    signal avatarClicked(Tlog tlog, Author author)
    Poppable {
        body: back
        Text {
            visible: !listView.visible && !feedModel.hasMore
            anchors.centerIn: parent
            color: window.secondaryTextColor
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: window.fontBigger
            wrapMode: Text.Wrap
            text: 'Нет записей'
        }
    }
    Splash {
        visible: !listView.visible && feedModel.hasMore
    }
    MyListView {
        id: listView
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: contentHeight > parent.height ? parent.height : contentHeight
        visible: count > 0
        onAboveBegin: feedModel.reset()
        Connections {
            target: back
            onPushed: {
//                console.log('pushed', listView.currentIndex)
                if (listView.currentIndex >= 0)
                    listView.positionViewAtIndex(listView.currentIndex, ListView.Contain)
            }
        }
        model: FeedModel {
            id: feedModel
            mode: back.mode
            tlog: back.tlogId
        }
        header: Item {
            anchors.left: parent.left
            anchors.right: parent.right
            height: 0
            Item {
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                height: 8 * mm
                Text {
                    text: 'Обновить'
                    color: window.secondaryTextColor
                    font.pointSize: window.fontBigger
                    anchors.fill: parent
                    anchors.margins: 1 * mm
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
        delegate: Item {
            id: entryView
            width: window.width
            property color fontColor: window.textColor
            height: 8 * mm + content.height + entryTitle.height + entryAvatar.height + comments.height
                    + firstImage.height + quoteSource.height
            function saveCurrentIndex() {
                listView.currentIndex = listView.indexAt(entryView.x + 1, entryView.y + 1);
            }
            Poppable {
                body: back
                onClicked: {
                    if (back.x > 0) {
                        mouse.accepted = false;
                        return;
                    }

                    saveCurrentIndex();
                    entryClicked(entry);
                }
            }
            SmallAvatar {
                id: entryAvatar
                anchors.margins: 1 * mm
                user: entry.author
                Poppable {
                    body: back
                    onClicked:
                    {
                        if (back.x > 0 || mode === FeedModel.AnonymousMode) {
                            mouse.accepted = false;
                            return;
                        }

                        saveCurrentIndex();
                        back.avatarClicked(entry.tlog, entry.author);
                    }
                }
            }
            Text {
                id: nick
                text: entry.author.name
                color: entryView.fontColor
                font.pointSize: window.fontSmaller
                anchors.top: parent.top
                anchors.left: entryAvatar.right
                anchors.right: entryVoteButton.left
                anchors.margins: 1 * mm
                elide: Text.AlignRight
                horizontalAlignment: Text.AlignLeft
            }
            Text {
                id: date
                text: entry.createdAt
                color: window.secondaryTextColor
                font.pointSize: window.fontSmallest
                anchors.top: nick.bottom
                anchors.left: entryAvatar.right
//                anchors.bottom: entryAvatar.bottom
                anchors.margins: 1 * mm
            }
            ThemedButton {
                id: entryVoteButton
                anchors.top: parent.top
                anchors.right: parent.right
                width: parent.width / 4
                height: entryAvatar.height
                text: '+ ' + entry.rating.votes
                visible: entry.isVotable && Tasty.isAuthorized
                enabled: entry.rating.isVotable
                checked: entry.rating.isVoted
                fontSize: 20
                onClicked: {
                    if (back.x > 0) {
                        mouse.accepted = false;
                        return;
                    }

                    entry.rating.vote();
                }
            }
            MyImage {
                id: firstImage
                property AttachedImage image: entry.attachedImagesModel.first()
                visible: image
                anchors.top: entryAvatar.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.topMargin: 1 * mm
                anchors.bottomMargin: 1 * mm
                height: visible ? (image.height / image.width * width) : 0
                url: visible ? image.url : ''
                extension: visible ? image.type : ''
                Text {
                    anchors.bottom: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.margins: 0.5 * mm
                    font.pointSize: window.fontSmallest
                    color: window.textColor
                    style: Text.Outline
                    styleColor: window.backgroundColor
                    property int total: entry.attachedImagesModel.rowCount()
                    text: Tasty.num2str(total, 'изображение', 'изображения', 'изображений')
                    visible: total > 1
                }
            }
            Text {
                id: entryTitle
                text: entry.truncatedTitle
                anchors.top: firstImage.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 1 * mm
                wrapMode: Text.Wrap
                font.pointSize: entry.truncatedText.length > 0 ? window.fontBigger
                                                               : window.fontNormal
                color: parent.fontColor
                textFormat: Text.RichText
                height: entry.truncatedTitle.length > 0 ? contentHeight
                                                        : entry.truncatedText.length > 0 ? -2 * mm : 0
            }
            Text {
                id: content
                text: entry.truncatedText
                anchors.top: entryTitle.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 1 * mm
                anchors.leftMargin: entry.type === 'quote' ? 5 * mm : 1 * mm
                anchors.rightMargin: anchors.leftMargin
                wrapMode: Text.Wrap
                font.pointSize: window.fontNormal
                color: parent.fontColor
                textFormat: Text.RichText
                height: entry.truncatedText.length > 0 ? contentHeight
                                                       : entry.truncatedTitle.length > 0 ? -2 * mm : 0
            }
            Text {
                id: quoteSource
                text: entry.source
                anchors.top: content.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 1 * mm
                wrapMode: Text.Wrap
                font.pointSize: window.fontSmaller
                font.italic: true
                color: parent.fontColor
                textFormat: Text.RichText
                height: entry.source.length > 0 ? contentHeight : 0
                horizontalAlignment: Text.AlignRight
            }
            Rectangle {
                id: wc
                anchors.left: parent.left
                anchors.verticalCenter: comments.verticalCenter
                anchors.margins: 1 * mm
                height: 0.5 * mm
                property int maxWidth: comments.x - 2 * mm
                property int length: Math.sqrt(entry.wordCount) / 32 * maxWidth
                width: length < maxWidth ? length : maxWidth
                color: window.secondaryTextColor
            }
            Text {
                id: comments
                text: entry.commentsCount + ' коммент.'
                color: entryView.fontColor
                font.pointSize: window.fontSmallest
                anchors.top: quoteSource.bottom
                anchors.right: parent.right
                anchors.margins: 1 * mm
            }
        }
    }
}
