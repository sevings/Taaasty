import QtQuick 2.3
import org.binque.taaasty 1.0

Rectangle {
    id: footer
    anchors.left: parent.left
    anchors.right: parent.right
    height: avatar.height
    y: 0
    visible: y >= - height
    property string title
    property Tlog tlog
    signal avatarClicked
    gradient: Gradient {
        GradientStop { position: 0; color: '#373737' }
        GradientStop { position: 1; color: '#000000' }
    }
    Behavior on y {
        NumberAnimation {
            duration: 200
        }
    }
    SmallAvatar {
        id: avatar
        anchors.left: parent.left
        user: footer.tlog.author
        defaultSource: 'http://taaasty.com/favicons/favicon-64x64.png'
    }
    Text {
        id: title
        text: footer.title || footer.tlog.author.name
        color: window.textColor
        font.pointSize: window.fontBigger
        anchors.left: avatar.right
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.margins: 1 * mm
        horizontalAlignment: Text.AlignLeft
        elide: Text.ElideRight
    }
    MouseArea {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: notifButton.visible ? notifButton.left : parent.right
        onClicked: {
            if (footer.tlog.tlogId > 0 && footer.tlog.tlogId !== window.anonymousId)
                footer.avatarClicked()
        }
    }
    MouseArea {
        id: notifButton
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: parent.height * 1.5
        visible: Tasty.isAuthorized
        onClicked: {
            window.toggleNotifs()
        }
        Rectangle {
            id: notifNotice
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.top
            color: window.unreadNotifications ? window.brightColor : '#575757'
            width: 1.5 * mm
            height: width
            radius: height / 2
        }
    }
}
