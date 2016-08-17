import QtQuick 2.7
import QtQuick.Controls.Material 2.0
import org.binque.taaasty 1.0

Popup {
    id: footer
    height: avatar.height
    closable: false
    property string title
    property Tlog tlog
    SmallAvatar {
        id: avatar
        user: footer.tlog.author
        defaultSource: 'http://taaasty.com/favicons/favicon-64x64.png'
    }
    ThemedText {
        id: title
        text: footer.title || footer.tlog.author.name
        font.pointSize: window.fontBigger
//        color: 'white'
        color: Material.foreground
        anchors {
            left: avatar.right
            right: parent.right
            verticalCenter: parent.verticalCenter
        }
        horizontalAlignment: Text.AlignLeft
        elide: Text.ElideRight
        wrapMode: Text.NoWrap
    }
    MouseArea {
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
            right: notifButton.visible ? notifButton.left : parent.right
        }
        onClicked: {
            if (window.notifsShows)
                window.hideNotifs();
            else if (footer.tlog.tlogId > 0 && footer.tlog.tlogId !== window.anonymousId)
                window.pushProfile(tlog, tlog.author)
        }
    }
    MouseArea {
        id: notifButton
        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }
        width: parent.height * 1.5
        visible: Tasty.isAuthorized
        onClicked: {
            window.toggleNotifs()
        }
        Rectangle {
            id: notifNotice
            anchors {
                horizontalCenter: parent.horizontalCenter
                verticalCenter: parent.top
            }
            color: window.unreadNotifications ? window.greenColor : (window.darkTheme ? '#575757' : '#8d9aa6')
            width: 1.5 * mm
            height: width
            radius: height / 2
        }
    }
}
