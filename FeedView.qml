import QtQuick 2.3
import org.binque.taaasty 1.0

Rectangle {
    id: back
    color: window.backgroundColor
    signal popped
    property bool poppable
    property int mode: FeedModel.LiveMode
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
    signal entryClicked(TlogEntry entry)
    signal avatarClicked(Tlog tlog, Author author)
    signal flowClicked(int flowId)
    function setMode(m, t, s) {
        if (mode === m) {
            query = '';
            feedModel.reset(m, t, s);
        }
        else {
            mode = m;
            if (m === FeedModel.TlogMode)
            {
                if (t && t > 0)
                    tlogId = t;
                if (s)
                    slug = s;
//                if (q)
            }
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
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: contentHeight > parent.height ? parent.height : contentHeight
        visible: count > 0
//        onAboveBegin: feedModel.reset()
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
            slug: back.slug
            minRating: back.minRating
            query: back.query
        }
//        header: Item {
//            anchors.left: parent.left
//            anchors.right: parent.right
//            height: 0
//            Item {
//                anchors.bottom: parent.bottom
//                anchors.left: parent.left
//                anchors.right: parent.right
//                height: 8 * mm
//                Text {
//                    text: 'Обновить'
//                    color: window.secondaryTextColor
//                    font.pointSize: window.fontBigger
//                    anchors.fill: parent
//                    anchors.margins: 1 * mm
//                    horizontalAlignment: Text.AlignHCenter
//                    verticalAlignment: Text.AlignVCenter
//                }
//            }
//        }
        delegate: Item {
            id: entryView
            width: window.width
            height: 15 * mm + content.height + entryTitle.height + entryAvatar.height + comments.height
                    + firstImage.height + quoteSource.height + repostText.height
                    + mediaLink.height + entryVoteButton.height
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
            ThemedText {
                id: repostText
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 5 * mm
                anchors.rightMargin: anchors.leftMargin
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
                        back.flowClicked(entry.tlog.author.id)
                    }
                }
            }
            SmallAvatar {
                id: entryAvatar
                anchors.top: repostText.bottom
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
            ThemedText {
                id: nick
                text: entry.author.name
                font.pointSize: window.fontSmaller
                anchors.top: repostText.bottom
                anchors.left: entryAvatar.right
                anchors.right: parent.right
                elide: Text.AlignRight
                wrapMode: Text.NoWrap
                horizontalAlignment: Text.AlignLeft
            }
            ThemedText {
                id: date
                text: entry.createdAt
                color: window.secondaryTextColor
                font.pointSize: window.fontSmallest
                anchors.top: nick.bottom
                anchors.left: entryAvatar.right
                anchors.right: parent.right
                elide: Text.AlignRight
                wrapMode: Text.NoWrap
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
                savable: true
                popBody: back
                ThemedText {
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.margins: 0.5 * mm
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
                anchors.top: firstImage.bottom
                anchors.left: parent.left
                anchors.right: parent.right
//                anchors.topMargin: 1 * mm
                anchors.bottomMargin: 1 * mm
                media: entry.media
            }
            ThemedText {
                id: entryTitle
                text: entry.truncatedTitle
                anchors.top: mediaLink.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                font.pointSize: entry.truncatedText.length > 0 ? window.fontBigger
                                                               : window.fontNormal
                textFormat: Text.RichText
                height: entry.truncatedTitle.length > 0 ? contentHeight
                                                        : entry.truncatedText.length > 0 ? -2 * mm : 0
            }
            ThemedText {
                id: content
                text: entry.truncatedText
                anchors.top: entryTitle.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: entry.type === 'quote' ? 5 * mm : 1 * mm
                anchors.rightMargin: anchors.leftMargin
                textFormat: Text.RichText
                height: entry.truncatedText.length > 0 ? contentHeight
                                                       : entry.truncatedTitle.length > 0 ? -2 * mm : 0
            }
            ThemedText {
                id: quoteSource
                text: entry.source
                anchors.top: content.bottom
                anchors.left: parent.left
                anchors.right: parent.right
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
                anchors.top: quoteSource.bottom
                anchors.right: parent.right
            }
            Rectangle {
                id: br
                anchors.left: parent.left
                anchors.bottom: comments.verticalCenter
                anchors.leftMargin: 1 * mm
                anchors.bottomMargin: 0.2 * mm
                height: 0.5 * mm
                property int maxWidth: comments.x - 2 * mm
                property int length: Math.abs(entry.rating.bayesRating) / 100 * maxWidth
                width: length < maxWidth ? length : maxWidth
                color: entry.rating.bayesRating > 0 ? window.darkGreen : window.darkRed
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
                anchors.left: parent.left
                anchors.top: comments.verticalCenter
                anchors.leftMargin: 1 * mm
                anchors.topMargin: 0.2 * mm
                height: 0.5 * mm
                property int maxWidth: comments.x - 2 * mm
                property int length: Math.sqrt(entry.wordCount) / 32 * maxWidth
                width: length < maxWidth ? length : maxWidth
                color: window.secondaryTextColor
            }
            ThemedButton {
                id: entryVoteButton
                anchors.top: comments.bottom
                anchors.right: parent.right
                width: parent.width / 3
                height: 6 * mm
                text: 'Да!'
//                visible: entry.isVotable && Tasty.isAuthorized
//                enabled: entry.rating.isVotable || (!entry.rating.isBayesVoted && !entry.isVotedAgainst)
                checked: entry.rating.isVoted || entry.rating.isBayesVoted
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
                anchors.top: comments.bottom
                anchors.left: parent.left
                height: entryVoteButton.height
                width: parent.width / 3
                text: 'Фу…'
//                visible: entry.isVotable && Tasty.isAuthorized
                enabled: !entry.rating.isBayesVoted && !entry.rating.isVotedAgainst
                checked: entry.rating.isVotedAgainst
                fontSize: 20
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
                text: entry.isVotable ? '+ ' + entry.rating.votes : ''
                anchors.top: comments.bottom
                anchors.left: entryVoteAgainstButton.right
                anchors.right: entryVoteButton.left
                height: entryVoteButton.height
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
