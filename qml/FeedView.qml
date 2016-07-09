import QtQuick 2.7
import org.binque.taaasty 1.0

Pane {
    id: back
    property int mode
    property int tlogId: 0
    property string slug: ''
    property int minRating: 0
    property string query: ''
    property Tlog tlog: Tlog {
        tlogId: back.tlogId
        slug: back.slug
    }
    readonly property string title: { // for footer
        switch (back.mode) {
        case FeedModel.MyTlogMode:
            'Мой тлог'; break;
        case FeedModel.LiveMode:
            'Прямой эфир'; break;
        case FeedModel.FriendsMode:
            'Подписки'; break;
        case FeedModel.BestMode:
            'Лучшее'; break;
        case FeedModel.ExcellentMode:
            'Отличное'; break;
        case FeedModel.GoodMode:
            'Хорошее'; break;
        case FeedModel.WellMode:
            'Неплохое'; break;
        case FeedModel.BetterThanMode:
            'Лучше, чем ' + minRating; break;
        case FeedModel.AnonymousMode:
            'Анонимки'; break;
        case FeedModel.FavoritesMode:
            'Избранное'; break;
        default:
            'Тейсти'
        }
    }
    readonly property bool customTitle: mode !== FeedModel.TlogMode// && mode !== FeedModel.MyTlogMode
    readonly property bool isFeedView: true
    signal pushed
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
        text: feedModel.isPrivate ? 'Это закрытый ' + (tlog.author.isFlow ? 'поток' : 'тлог') : 'Загрузка…'
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
        Connections {
            target: back
            onPushed: {
//                console.log('pushed', listView.currentIndex)
//                if (listView.currentIndex >= 0)
//                    listView.positionViewAtIndex(listView.currentIndex, ListView.Contain)
            }
        }
        model: FeedModel {
            id: feedModel
            mode: back.mode
            tlog: back.tlogId
            slug: back.slug
            minRating: back.minRating
            query: back.query
        }
        delegate: Item {
            id: entryView
            width: window.width
            height: 7 * mm + entryVoteButton.y + entryVoteButton.height
            function saveCurrentIndex() {
//                listView.currentIndex = index;
            }
            Poppable {
                body: back
                onClicked: {
                    if (back.x > 0) {
                        mouse.accepted = false;
                        return;
                    }

                    mouse.accepted = true;

//                    saveCurrentIndex();
                    window.pushFullEntry(entry);
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
                height: visible ? contentHeight : -mm
                horizontalAlignment: Text.AlignHCenter
                text: entry.tlog.author.name
                      + (entry.tlog.author.title ? '\n' + entry.tlog.author.title : '')
                Poppable {
                    body: back
                    onClicked: {
                        mouse.accepted = true;
                        window.pushTlog(entry.tlog.author.id);
                    }
                }
            }
            SmallAvatar {
                id: entryAvatar
                anchors {
                    top: repostText.bottom
                    margins: 1 * mm
                }
                user: entry.author
                Poppable {
                    body: back
                    onClicked:
                    {
                        if (back.x > 0 || mode === FeedModel.AnonymousMode) {
                            mouse.accepted = false;
                            return;
                        }

                        mouse.accepted = true;

//                        saveCurrentIndex();
                        window.pushProfile(entry.tlog, entry.author);
                    }
                }
            }
            ThemedText {
                id: nick
                text: entry.author.name
                font.pointSize: window.fontSmaller
                anchors {
                    top: repostText.bottom
                    left: entryAvatar.right
                    right: parent.right
                }
                elide: Text.AlignRight
                wrapMode: Text.NoWrap
                horizontalAlignment: Text.AlignLeft
            }
            ThemedText {
                id: date
                text: entry.createdAt
                color: window.secondaryTextColor
                font.pointSize: window.fontSmallest
                anchors {
                    top: nick.bottom
                    left: entryAvatar.right
                    right: parent.right
                }
                elide: Text.AlignRight
                wrapMode: Text.NoWrap
            }
            MyImage {
                id: firstImage
                property AttachedImage image: entry.attachedImagesModel.first()
                visible: image
                anchors {
                    top: entryAvatar.bottom
                    left: parent.left
                    right: parent.right
                    topMargin: 1 * mm
                    bottomMargin: 1 * mm
                }
                height: visible ? (image.height / image.width * width) : 0
                url: visible ? image.url : ''
                extension: visible ? image.type : ''
                savable: true
                popBody: back
                ThemedText {
                    anchors {
                        bottom: parent.bottom
                        left: parent.left
                        right: parent.right
                        margins: 0.5 * mm
                    }
                    horizontalAlignment: Text.AlignHCenter
                    font.pointSize: window.fontSmaller
                    style: Text.Outline
                    styleColor: window.backgroundColor
                    property int total: entry.attachedImagesModel.rowCount()
                    text: Tasty.num2str(total, 'изображение', 'изображения', 'изображений')
                    visible: total > 1
                }
            }
            MediaLink {
                id: mediaLink
                visible: entry.media
                anchors {
                    top: firstImage.bottom
                    left: parent.left
                    right: parent.right
                    bottomMargin: 1 * mm
                }
                media: entry.media
            }
            ThemedText {
                id: entryTitle
                text: entry.truncatedTitle
                anchors {
                    top: mediaLink.bottom
                    left: parent.left
                    right: parent.right
                }
                font.pointSize: entry.truncatedText.length > 0 ? window.fontBigger
                                                               : window.fontNormal
                textFormat: Text.RichText
                height: entry.truncatedTitle.length > 0 ? contentHeight
                                                        : entry.truncatedText.length > 0 ? -2 * mm : 0
            }
            ThemedText {
                id: content
                text: entry.truncatedText
                anchors {
                    top: entryTitle.bottom
                    left: parent.left
                    right: parent.right
                    leftMargin: entry.type === 'quote' ? 5 * mm : 1 * mm
                    rightMargin: anchors.leftMargin
                }
                textFormat: Text.RichText
                height: entry.truncatedText.length > 0 ? contentHeight
                                                       : entry.truncatedTitle.length > 0 ? -2 * mm : 0
            }
            ThemedText {
                id: quoteSource
                text: entry.source
                anchors {
                    top: content.bottom
                    left: parent.left
                    right: parent.right
                }
                font.pointSize: window.fontSmaller
                font.italic: true
                textFormat: Text.RichText
                height: entry.source.length > 0 ? contentHeight : 0
                horizontalAlignment: Text.AlignRight
            }
            ThemedText {
                id: comments
                text: entry.commentsCount + ' коммент.'
                font.pointSize: window.fontSmallest
                anchors {
                    top: quoteSource.bottom
                    right: parent.right
                }
            }
            Rectangle {
                id: br
                anchors {
                    left: parent.left
                    bottom: comments.verticalCenter
                    leftMargin: 1 * mm
                    bottomMargin: 0.2 * mm
                }
                height: 0.5 * mm
                property int maxWidth: comments.x - 2 * mm
                property int length: Math.abs(entry.rating.bayesRating) / 100 * maxWidth
                width: length < maxWidth ? length : maxWidth
                color: entry.rating.bayesRating > 0 ? (window.darkTheme ? window.darkGreen : window.greenColor)
                                                    : (window.darkTheme ? window.darkRed : window.redColor)
                Behavior on width {
                    NumberAnimation {
                        duration: 300
                    }
                }
                Behavior on color {
                    ColorAnimation {
                        duration: 300
                    }
                }
            }
            Rectangle {
                id: wc
                anchors {
                    left: parent.left
                    top: comments.verticalCenter
                    leftMargin: 1 * mm
                    topMargin: 0.2 * mm
                }
                height: 0.5 * mm
                property int maxWidth: comments.x - 2 * mm
                property int length: Math.sqrt(entry.wordCount) / 32 * maxWidth
                width: length < maxWidth ? length : maxWidth
                color: window.secondaryTextColor
            }
            ThemedButton {
                id: entryVoteButton
                anchors {
                    top: comments.bottom
                    right: parent.right
                }
                width: parent.width / 3
                height: 6 * mm
                text: 'Да!'
                enabled: !entry.rating.isVotedAgainst || entry.rating.isVotable
                glowing: ((entry.rating.isVotable === entry.rating.isVoted) )
                         && entry.rating.isBayesVoted
                onClicked: {
                    if (back.x > 0) {
                        mouse.accepted = false;
                        return;
                    }

                    entry.rating.vote();
                }
            }
            ThemedButton {
                id: entryVoteAgainstButton
                anchors {
                    top: comments.bottom
                    left: parent.left
                }
                height: entryVoteButton.height
                width: parent.width / 3
                text: 'Фу…'
                enabled: !entry.rating.isBayesVoted
                glowing: entry.rating.isVotedAgainst
                glowColor: window.darkTheme ? window.darkRed : window.redColor
                onClicked: {
                    if (back.x > 0) {
                        mouse.accepted = false;
                        return;
                    }

                    entry.rating.voteAgainst();
                }
            }
            ThemedText {
                id: entryRating
                anchors {
                    top: comments.bottom
                    left: entryVoteAgainstButton.right
                    right: entryVoteButton.left
                }
                text: entry.isVotable ? '+ ' + entry.rating.votes : ''
                height: entryVoteButton.height
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
