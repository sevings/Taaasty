import QtQuick 2.7
import QtQuick.Controls 2.0 as Q
import QtQuick.Controls.Material 2.0
//import QtGraphicalEffects 1.0
import org.binque.taaasty 1.0

Pane {
    id: back
    property int entryId
    property TlogEntry entry: TlogEntry {
        entryId: back.entryId
    }
    readonly property Tlog tlog: entry.tlog
    property CommentsModel commentsModel: entry.comments
    property bool showProfiles: tlog.tlogId !== window.anonymousId
    property bool scrollToBottom: false
    readonly property bool isFullEntryView: true
    Timer {
        interval: 20000
        running: back.visible && !entry.chat.isInvolved
        repeat: true
        onTriggered: commentsModel.check()
    }
    Poppable {
        body: back
    }
    Splash {
        visible: !fullEntry.visible
    }
    signal addGreeting(string slug)
    Connections {
        target: entry
        onUpdated: {
            window.setFooterTlog(entry.tlog);
            commentsModel.check();
            if (scrollToBottom)
                fullEntry.positionViewAtEnd();
        }
    }
    Component.onCompleted: {
        commentsModel.check();
        if (scrollToBottom)
            fullEntry.positionViewAtEnd();
    }
    MyListView {
        id: fullEntry
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: contentHeight > parent.height ? parent.height : contentHeight
        visible: !entry.loading
        model: commentsModel
        delegate: Item {
            width: window.width
            height: 4 * mm + commentText.contentHeight + nameText.contentHeight
            Component.onCompleted: {
                entry.chat.readAll();
            }
            Poppable {
                body: back
                onClicked: {
                    menu.show(comment);
                }
            }
            SmallAvatar {
                id: commentAvatar
                anchors.margins: 1 * mm
                user: comment.user
                Poppable {
                    body: back
                    onClicked: {
                        mouse.accepted = true;
                        if (back.showProfiles)
                            window.pushProfileById(comment.user.id);
                    }
                }
            }
            Q.Label {
                id: nameText
                text: comment.user.name
                anchors {
                    top: parent.top
                    left: commentAvatar.right
                    right: commentDate.left
                    leftMargin: 1 * mm
                    rightMargin: 1 * mm
                }
                wrapMode: Text.Wrap
                font.pointSize: window.fontSmaller
                font.bold: true
                style: Text.Raised
                styleColor: window.greenColor
            }
            Q.Label {
                id: commentDate
                text: comment.createdAt
                color: window.secondaryTextColor
                anchors {
                    baseline: nameText.baseline
                    right: parent.right
                    leftMargin: 1 * mm
                    rightMargin: 1 * mm
                }
                font.pointSize: window.fontSmaller
            }
            Q.Label {
                id: commentText
                text: comment.text
                anchors {
                    rightMargin: 1 * mm
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
            height: 10 * mm + fullTitle.height + fullContent.height + quoteSource.height
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
                    bottomMargin: 1 * mm
                }
                interactive: false
                spacing: 1 * mm
                property AttachedImagesModel imagesModel: entry.attachedImagesModel
                height: imagesModel ? (imagesModel.listRatio() * window.width
                        + (imagesModel.rowCount() - 1) * 10) : 0
                model: imagesModel
                delegate: MyImage {
                    id: picture
                    anchors {
                        topMargin: 1 * mm
                        bottomMargin: 1 * mm
                    }
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
                    bottomMargin: 1 * mm
                }
                media: entry.media
                Poppable {
                    body: back
                    onClicked: {
                        if (!mediaLink.source.toString() && mediaLink.url)
                            mouse.accepted = false;
                        else
                            mediaLink.play();
                    }
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
                height: entry.title.length > 0 ? contentHeight : entry.text.length > 0 ? -1 * mm : 0
                onLinkActivated: window.openLink(link)
            }
            ThemedText {
                id: fullContent
                text: entry.text
                anchors {
                    top: fullTitle.bottom
                    left: parent.left
                    right: parent.right
                    leftMargin: entry.type === 'quote' ? 5 * mm : 1 * mm
                    rightMargin: anchors.leftMargin
                }
                textFormat: Text.RichText
                height: entry.text.length > 0 ? contentHeight : entry.title.length > 0 ? -1 * mm : 0
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
                height: entry.source.length > 0 ? contentHeight : -1 * mm
                horizontalAlignment: Text.AlignRight
            }
            Text {
                id: fullEntryDate
                anchors {
                    top: quoteSource.bottom
                    left: parent.left
                    margins: 1 * mm
                }
                text: entry.createdAt
                color: window.secondaryTextColor
                font.pointSize: window.fontSmallest
            }
            Q.Label {
                id: fullEntryCommentsCount
                anchors {
                    top: quoteSource.bottom
                    right: parent.right
                    margins: 1 * mm
                }
                text: entry.commentsCount + ' коммент.'
                color: window.secondaryTextColor
                font.pointSize: window.fontSmallest
            }
            Rectangle {
                id: br
                anchors {
                    top: fullEntryDate.bottom
                    left: parent.left
                    margins: 1 * mm
                }
                height: 0.5 * mm
                property int maxWidth: window.width - 2 * mm
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
            ThemedButton {
                id: fullEntryVoteAgainstButton
                anchors {
                    top: br.bottom
                    left: parent.left
                    margins: 1 * mm
                }
                text: '-'
//                height: 6 * mm
                width: parent.width / 3
                enabled: !entry.rating.isBayesVoted
                highlighted: entry.rating.isVotedAgainst
                Material.accent: Material.Red
                onClicked: entry.rating.voteAgainst()
            }
            ThemedButton {
                id: fullEntryVoteButton
                anchors {
                    top: br.bottom
                    right: parent.right
                    margins: 1 * mm
                }
                text: '+'
//                height: 6 * mm
                width: parent.width / 3
                enabled: !entry.rating.isVotedAgainst
                highlighted: ((entry.rating.isVotable === entry.rating.isVoted) )
                             && entry.rating.isBayesVoted
                Material.accent: Material.Green
                onClicked: entry.rating.vote()
            }
            ThemedText {
                id: fullEntryRating
                anchors {
                    top: br.bottom
                    left: fullEntryVoteAgainstButton.right
                    right: fullEntryVoteButton.left
                }
                text: entry.isVotable ? '+ ' + entry.rating.votes : ''
                height: fullEntryVoteButton.height
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            ThemedButton {
                id: loadMoreButton
                anchors {
                    top: fullEntryVoteButton.bottom
                    horizontalCenter: parent.horizontalCenter
                    margins: 1 * mm
                    topMargin: 2 * mm
                }
                text: enabled || fullEntry.count > 0 ? 'Еще' : ''
                height: visible ? implicitHeight : 0 // changing height forces layout
                width: parent.width / 3
                visible: commentsModel && commentsModel.hasMore && !commentsModel.loading
                onClicked: commentsModel.loadMore()
            }
            Q.BusyIndicator {
                id: busy
                anchors {
                    top: fullEntryVoteButton.bottom
                    horizontalCenter: parent.horizontalCenter
                    margins: 1 * mm
                    topMargin: 2 * mm
                }
                running: commentsModel && commentsModel.loading
            }
        }
        footer: MessageEditor {
            id: commentEditor
            visible: Tasty.isAuthorized
            height: visible ? implicitHeight : - 1 * mm
            onSent: {
                entry.addComment(commentEditor.message);
            }
            Connections {
                target: entry
                onCommentAdded: {
                    commentEditor.clear();
                }
            }
            Connections {
                target: back
                onAddGreeting: {
                    commentEditor.addGreeting(slug);
                }
            }
        }
    }
//    DropShadow {
//        anchors.fill: menu
//        horizontalOffset: 0.6 * mm
//        verticalOffset: 0.6 * mm
//        samples: 1
//        color: "#80000000"
//        source: menu
//        cached: true
//    }
    Popup {
        id: menu
        height: menuColumn.height + 2 * mm
//        border.color: window.secondaryTextColor
//        border.width: 0.2 * mm
//        radius: 0.8 * mm
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
                topMargin: 1 * mm
                bottomMargin: 1 * mm
            }
            spacing: 1 * mm
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
