import QtQuick 2.5
import QtQuick.Controls 1.4

MyListView {
    delegate: Item {
        id: entry
        anchors.left: parent.left
        anchors.right: parent.right
//        color: window.backgroundColor
        property color fontColor: window.textColor
        height: 50 + content.height + entryTitle.height + entryAvatar.height + comments.height// + images.height
        //                    MouseArea {
        //                        anchors.fill: parent
        //                        onClicked: {
        //                            var ix =  parent.x + mouseX;
        //                            var iy =  parent.y + mouseY;
        //                            thisTlog.currentIndex = thisTlog.indexAt(ix, iy);
        //                            Ctrl.showFullEntry(thisTlog.model.get(thisTlog.currentIndex));
        //                        }
        //                    }
        SmallAvatar {
            id: entryAvatar
            anchors.margins: 10
            source: !symbol ? author.userpic.thumb64_url : ''
            name: author.name
            symbol: !author.userpic.hasOwnProperty('thumb64_url')
        }
        Text {
            id: nick
            text: author.name
            color: entry.fontColor
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
            text: createdAt
            color: window.secondaryTextColor
            font.pointSize: 14
            anchors.top: nick.bottom
            anchors.left: entryAvatar.right
            anchors.margins: 10
        }
        //                    function updateRating(eid, newRating) {
        //                        rating.votes = newRating.votes
        //                        rating.is_voted = newRating.is_voted;
        //                        entryVoteButton.label = '+ ' + newRating.votes;
        //                        entryVoteButton.enabled = can_vote && !newRating.is_voted;
        //                    }
        ThemedButton {
            id: entryVoteButton
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.margins: 10
            height: 64
            width: parent.width / 5
            text: '+ ' + rating.votes
            visible: isVotable
            enabled: isVotable && !rating.is_voted
            fontSize: 20
//            onClicked: {
//                var ix =  parent.x + mouseX;
//                var iy =  parent.y + mouseY;
//                //console.log('x: ' + ix + ', y: ' + iy);
//                thisTlog.currentIndex = thisTlog.indexAt(ix, iy);
//                Ctrl.voteForEntry(thisTlog.model.get(thisTlog.currentIndex).id, rating.is_voted);
//            }
        }

//         ImagesView {
//             id: images
//             anchors.top: entryAvatar.bottom
//             model: attach
//             height: wholeHeight * width
//         }
        Text {
            id: entryTitle
            text: truncatedTitle
//            anchors.top: images.bottom
            anchors.top: entryAvatar.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 10
            wrapMode: Text.Wrap
//            font.family: tlog.design.feedFont
            font.pointSize: truncatedText.length > 0 ? 25 : 20
            color: parent.fontColor
            textFormat: Text.RichText
            height: truncatedTitle.length > 0 ? paintedHeight : truncatedText.length > 0 ? -20 : 0
        }
        Text {
            id: content
            text: truncatedText
            anchors.top: entryTitle.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 10
            wrapMode: Text.Wrap
//            font.family: tlog.design.feedFont
            font.pointSize: 20
            color: parent.fontColor
            textFormat: Text.RichText
            height: truncatedText.length > 0 ? paintedHeight : truncatedTitle.length > 0 ? -20 : 0
        }
        Text {
            id: comments
            text: commentsCount + ' коммент.'
            color: entry.fontColor
            font.pointSize: 15
            anchors.top: content.bottom
            anchors.right: parent.right
            anchors.margins: 10
        }
    }
}
