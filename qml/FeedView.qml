import QtQuick 2.7
import QtQuick.Controls.Material 2.0
import org.binque.taaasty 1.0

Pane {
    id: back
    innerFlick: listView
    property int mode: Tasty.isAuthorized ? FeedModel.FriendsMode : FeedModel.LiveMode
    readonly property bool bestMode: back.mode === FeedModel.BestMode
                                     || back.mode === FeedModel.ExcellentMode
                                     || back.mode === FeedModel.WellMode
                                     || back.mode === FeedModel.GoodMode
                                     || back.mode === FeedModel.BetterThanMode
    property int tlogId: 0
    property int sinceId: 0
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
//    signal pushed
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
        visible: !listView.visible && feedModel.hasMore && !pauseAnimations
        running: !feedModel.isPrivate
        text: 'Это закрытый ' + (tlog.author.isFlow ? 'поток' : 'тлог')
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
        interactive: back.x == 0
//        Connections {
//            target: back
//            onPushed: {
//                console.log('pushed', listView.currentIndex)
//                if (listView.currentIndex >= 0)
//                    listView.positionViewAtIndex(listView.currentIndex, ListView.Contain)
//            }
//        }
        Component.onCompleted: {
            if (back.tlogId && back.sinceId)
                feedModel.setSinceEntryId(back.sinceId);
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
                        if (back.x > 0) {
                            mouse.accepted = false;
                            return;
                        }

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
                paused: pauseAnimations
                popBody: back
                visible: mode !== FeedModel.AnonymousMode
                onClicked: {
                    if (back.x > 0 || mode === FeedModel.AnonymousMode) {
                        mouse.accepted = false;
                        return;
                    }

//                    saveCurrentIndex();
                    window.pushProfile(entry.tlog, entry.author);
                }
            }
            ThemedText {
                id: nick
                anchors {
                    top: repostText.bottom
                    left: entryAvatar.right
                    right: parent.right
                }
                text: entry.author.name
                font.pointSize: window.fontSmaller
                elide: Text.ElideRight
                wrapMode: Text.NoWrap
                horizontalAlignment: Text.AlignLeft
                visible: mode !== FeedModel.AnonymousMode
            }
            ThemedText {
                id: date
                anchors {
                    top: nick.visible ? nick.bottom : repostText.bottom
                    left: entryAvatar.visible ? entryAvatar.right : parent.left
                    right: parent.right
                }
                text: entry.createdAt
                color: window.secondaryTextColor
                font.pointSize: window.fontSmallest
                elide: Text.AlignRight
                wrapMode: Text.NoWrap
            }
            Loader {
                id: firstImage
                anchors {
                    top: entryAvatar.visible ? entryAvatar.bottom : date.bottom
                    left: parent.left
                    right: parent.right
                    topMargin: 1 * mm
                    bottomMargin: 1 * mm
                }
                property AttachedImage image: entry.attachedImagesModel.first()
                active: image
                height: active ? (image.height / image.width * width) : 0
                onLoaded: item.image = image
                sourceComponent: MyImage {
                    property AttachedImage image
                    url: image ? image.url : ''
                    extension: image ? image.type : ''
                    savable: true
                    popBody: back
                    paused: pauseAnimations
                    acceptClick: false
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
            }
            Loader {
                id: mediaLink
                property Media media: entry.media
                active: media
                height: media ? media.thumbnail.height / media.thumbnail.width * width : -anchors.topMargin
                anchors {
                    top: firstImage.bottom
                    left: parent.left
                    right: parent.right
                    bottomMargin: 1 * mm
                }
                onLoaded: item.media = media
                sourceComponent: MediaLink {
                    paused: pauseAnimations
                    acceptClick: false
                    popBody: back
                }
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
                visible: entry.type !== 'quote'
                height: visible && entry.truncatedTitle.length > 0
                        ? contentHeight : entry.truncatedText.length > 0 ? -2 * mm : 0
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
            Rectangle {
                id: wc
                anchors {
                    top: quoteSource.bottom
                    left: parent.left
                    margins: 1 * mm
                }
                height: 0.5 * mm
                property int maxWidth: parent.width - 2 * mm
                property int length: Math.sqrt(entry.wordCount) / 32 * maxWidth
                width: length < maxWidth ? length : maxWidth
                color: window.secondaryTextColor
            }
            Image {
                id: entryCommentsIcon
                anchors {
                    margins: 2 * mm
                    verticalCenter: comments.verticalCenter
                    left: parent.left
                }
                sourceSize.height: 36
                source: window.darkTheme ? '../icons/comment-light.svg'
                                         : '../icons/comment-dark.svg'
            }
            ThemedText {
                id: comments
                anchors {
                    top: wc.bottom
                    left: entryCommentsIcon.right
                }
                text: entry.commentsCount
                height: entryVoteButton.height
                color: window.secondaryTextColor
                verticalAlignment: Text.AlignVCenter
            }
            IconButton {
                id: entryVoteButton
                anchors {
                    top: wc.bottom
                    right: parent.right
                }
                icon: ((entry.rating.isVotable === entry.rating.isVoted) )
                      && entry.rating.isBayesVoted ? '../icons/flame-solid.svg'
                                                   : '../icons/flame-outline.svg'
                enabled: !entry.rating.isVotedAgainst || entry.rating.isVotable
                onClicked: {
                    entry.rating.vote();
                }
            }
            IconButton {
                id: entryVoteAgainstButton
                anchors {
                    top: wc.bottom
                    right: entryRating.left
                }
                icon: entry.rating.isVotedAgainst ? '../icons/drop-solid.svg'
                                                  : '../icons/drop-outline.svg'
                enabled: !entry.rating.isBayesVoted
                onClicked: {
                    entry.rating.voteAgainst();
                }
            }
            ThemedText {
                id: entryRating
                anchors {
                    top: wc.bottom
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
                }
                height: entry.isVotable ? 0.5 * mm : width
                radius: height / 2
                color: entry.rating.bayesRating > 0 ? '#FF5722' : '#03A9F4'
                Behavior on color {
                    ColorAnimation {
                        duration: 300
                    }
                }
            }
        }
        footer: ListBusyIndicator {
            running: feedModel.loading && !pauseAnimations
            visible: feedModel.hasMore
        }
    }
}
