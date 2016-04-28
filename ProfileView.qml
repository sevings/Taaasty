import QtQuick 2.3
//import QtQuick.Controls 1.2
import org.binque.taaasty 1.0

Rectangle {
    id: profileView
    color: window.backgroundColor
    property int tlogId
    property Tlog tlog: Tlog {
        tlogId: profileView.tlogId
    }
    property Author author: tlog.author
    signal tlogRequested
    signal followersRequested
    signal followingsRequested
    signal popped
    property bool poppable
    Flickable {
        id: flickable
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick
        bottomMargin: 10
        contentWidth: parent.width
        contentHeight: column.height
        Poppable {
            body: profileView
        }
        Column {
            id: column
            anchors.left: parent.left
            anchors.right: parent.right
            spacing: 10
            MyImage {
                id: bigAvatar
                anchors.topMargin: 20
                anchors.bottomMargin: 20
                anchors.left: parent.left
                anchors.right: parent.right
                height: width
                source: author.largePic
            }
            Text {
                id: name
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 10
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
                anchors.margins: 10
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
                anchors.margins: 10
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
                anchors.margins: 10
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
                anchors.margins: 10
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
                anchors.margins: 10
                font.pointSize: window.fontNormal
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
                color: window.textColor
                text: tlog.amIFollowing ? 'Вы подписаны' : 'Вы не подписаны'
                height: text.length > 0 ? paintedHeight : 0
                visible: tlog.id === author.id
            }
            Text {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 10
                font.pointSize: window.fontNormal
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
                color: window.textColor
                text: tlog.isFollowingMe ? 'Следит за вашим тлогом'
                                         : 'Не следит за вашим тлогом'
                height: text.length > 0 ? paintedHeight : 0
                visible: tlog.id === author.id && !author.isFlow
            }
            ThemedButton {
                anchors.left: parent.left
                anchors.right: parent.right
                text: author.isFlow ? 'Читать' : author.entriesCount
                onClicked: profileView.tlogRequested()
            }
            ThemedButton {
                anchors.left: parent.left
                anchors.right: parent.right
                text: tlog.followersCount
                onClicked: profileView.followersRequested()
            }
            ThemedButton {
                anchors.left: parent.left
                anchors.right: parent.right
                text: tlog.followingsCount
                visible: !author.isFlow
                onClicked: profileView.followingsRequested()
            }
        }
    }
}
