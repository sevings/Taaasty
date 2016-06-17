import QtQuick 2.7
import org.binque.taaasty 1.0

Rectangle {
    id: profileView
    color: window.backgroundColor
    property int tlogId
    property string slug: ''
    property Tlog tlog: Tlog {
        tlogId: profileView.tlogId
        slug: profileView.slug
    }
    property Author author: tlog.author
    signal tlogRequested
    signal followersRequested
    signal followingsRequested
    signal popped
    property bool poppable
    Splash {
        visible: !column.visible
    }
    Component.onCompleted: {
        if (tlog.tlogId && tlog.slug)
            tlog.author.checkStatus();
    }
    Flickable {
        id: flickable
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick
        bottomMargin: 1 * mm
        contentWidth: parent.width
        contentHeight: column.height
        Poppable {
            body: profileView
        }
        Column {
            id: column
            anchors.left: parent.left
            anchors.right: parent.right
            spacing: 1 * mm
            visible: !tlog.loading
            MyImage {
                id: bigAvatar
                anchors.topMargin: 2 * mm
                anchors.bottomMargin: 2 * mm
                anchors.left: parent.left
                anchors.right: parent.right
                height: width
                url: author.largePic
                savable: true
                popBody: profileView
            }
            ThemedText {
                id: name
                anchors.left: parent.left
                anchors.right: parent.right
                font.pointSize: window.fontBiggest
                horizontalAlignment: Text.AlignHCenter
                text: author.name
                height: text.length > 0 ? paintedHeight : 0
            }
            ThemedText {
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter
                text: author.title
                height: text.length > 0 ? paintedHeight : 0
            }
            ThemedText {
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter
                text: author.isFemale ? 'Девушка' : 'Парень'
                height: text.length > 0 ? paintedHeight : 0
                visible: !author.isFlow
            }
            ThemedText {
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter
                text: author.lastSeenAt
                height: text.length > 0 ? paintedHeight : 0
                visible: !author.isFlow
            }
            ThemedText {
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter
                text: author.isPrivacy ? 'Закрытый тлог' : 'Открытый тлог'
                height: text.length > 0 ? paintedHeight : 0
                visible: !author.isFlow
            }
            ThemedText {
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter
                text: author.daysCount
                height: text.length > 0 ? paintedHeight : 0
            }
            ThemedText {
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter
                text: tlog.isMe ? 'Это вы' : (tlog.amIFollowing ? 'Вы подписаны' : 'Вы не подписаны')
                height: text.length > 0 ? paintedHeight : 0
                visible: tlog.tlogId === author.id && Tasty.isAuthorized
            }
            ThemedText {
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter
                text: tlog.isFollowingMe ? 'Следит за вашим тлогом'
                                         : 'Не следит за вашим тлогом'
                height: text.length > 0 ? paintedHeight : 0
                visible: tlog.tlogId === author.id && !author.isFlow
                         && Tasty.isAuthorized && !tlog.isMe
            }
            ThemedButton {
                anchors.left: parent.left
                anchors.right: parent.right
                text: author.isFlow ? 'Читать' : author.publicEntriesCount
                onClicked: profileView.tlogRequested()
                enabled: !author.isPrivacy || tlog.amIFollowing || tlog.isMe
            }
            ThemedButton {
                anchors.left: parent.left
                anchors.right: parent.right
                text: tlog.followersCount
                onClicked: profileView.followersRequested()
                visible: tlog.tlogId === author.id && !author.isFlow
            }
            ThemedButton {
                anchors.left: parent.left
                anchors.right: parent.right
                text: tlog.followingsCount
                onClicked: profileView.followingsRequested()
                visible: tlog.tlogId === author.id && !author.isFlow
            }
        }
    }
}
