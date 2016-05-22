import QtQuick 2.5
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
//    Component.onCompleted: console.log(tlog.tlogId)
    Splash {
        visible: !column.visible
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
            }
            Text {
                id: name
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 1 * mm
                font.pointSize: window.fontBiggest
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
                color: window.textColor
                text: author.name
                height: text.length > 0 ? paintedHeight : 0
            }
            Text {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 1 * mm
                font.pointSize: window.fontNormal
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
                color: window.textColor
                text: author.title
                height: text.length > 0 ? paintedHeight : 0
            }
            Text {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 1 * mm
                font.pointSize: window.fontNormal
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
                color: window.textColor
                text: author.isFemale ? 'Девушка' : 'Парень'
                height: text.length > 0 ? paintedHeight : 0
                visible: !author.isFlow
            }
            Text {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 1 * mm
                font.pointSize: window.fontNormal
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
                color: window.textColor
                text: author.isPrivacy ? 'Закрытый тлог' : 'Открытый тлог'
                height: text.length > 0 ? paintedHeight : 0
                visible: !author.isFlow
            }
            Text {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 1 * mm
                font.pointSize: window.fontNormal
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
                color: window.textColor
                text: author.daysCount
                height: text.length > 0 ? paintedHeight : 0
            }
            Text {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 1 * mm
                font.pointSize: window.fontNormal
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
                color: window.textColor
                text: tlog.isMe ? 'Это вы' : (tlog.amIFollowing ? 'Вы подписаны' : 'Вы не подписаны')
                height: text.length > 0 ? paintedHeight : 0
                visible: tlog.tlogId === author.id && Tasty.isAuthorized
            }
            Text {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 1 * mm
                font.pointSize: window.fontNormal
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
                color: window.textColor
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
