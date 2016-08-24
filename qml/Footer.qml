import QtQuick 2.7
import QtQuick.Controls.Material 2.0
import org.binque.taaasty 1.0

Popup {
    id: footer
    height: 7 * mm
    closable: false

    property string title
    property Tlog tlog

    MouseArea {
        anchors.fill: parent
        onClicked: {
            mouse.accepted = true;
        }
        onDoubleClicked: {
            mouse.accepted = true;
        }
    }

    IconButton {
        id: left
        anchors {
            margins: 0
            left: parent.left
            top: parent.top
        }
        height: parent.height
        width: parent.width / 4
        iconHeight: 32
        icon: '../icons/' + (window.stackSize > 1 || window.notifsShows ? 'left-arrow-' : 'menu-rounded-')
              + (window.darkTheme ? 'white-' : 'black-')
              + (dp < 2 ? '32' : '64') + '.png'
        onClicked: {
            if (window.notifsShows)
                window.hideNotifs();
            else if (window.stackSize > 1)
                window.popFromStack();
            else
                forwardAnimation.start();
        }
    }
    IconButton {
        id: chats
        anchors {
            margins: 0
            left: left.right
            top: parent.top
        }
        height: parent.height
        width: parent.width / 4
        visible: Tasty.isAuthorized
        iconHeight: 32
        icon: '../icons/chat-'
              + (window.darkTheme ? 'white-' : 'black-')
              + (dp < 2 ? '32' : '64') + '.png'
        onClicked: {
            console.log('chats')
        }
        ThemedText {
            anchors {
                topMargin: 0
                top: parent.top
                right: parent.right
            }
            text: '<b>+' + Tasty.unreadChats + '</b>'
            visible: Tasty.unreadChats > 0
            style: Text.Outline
            styleColor: window.backgroundColor
            font.pointSize: window.fontBigger
        }
    }
    IconButton {
        id: bell
        anchors {
            margins: 0
            left: chats.right
            top: parent.top
        }
        height: parent.height
        width: parent.width / 4
        visible: Tasty.isAuthorized
        iconHeight: 32
        icon: '../icons/bell-'
              + (window.darkTheme ? 'white-' : 'black-')
              + (dp < 2 ? '32' : '64') + '.png'
        onClicked: {
            window.toggleNotifs();
        }
        ThemedText {
            anchors {
                topMargin: 0
                top: parent.top
                right: parent.right
            }
            text: '<b>+' + Tasty.unreadNotifications + '</b>'
            visible: Tasty.unreadNotifications > 0
            style: Text.Outline
            styleColor: window.backgroundColor
            font.pointSize: window.fontBigger
        }
    }
    IconButton {
        id: right
        anchors {
            margins: 0
            right: parent.right
            top: parent.top
        }
        height: parent.height
        width: parent.width / 4
        visible: window.canShowPageMenu
        iconHeight: 32
        icon: '../icons/menu-'
              + (window.darkTheme ? 'white-' : 'black-')
              + (dp < 2 ? '32' : '64') + '.png'
        onClicked: {
            window.showPageMenu();
        }
    }

/*
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
            color: window.unreadNotifications ? Material.accent : Material.background
            width: window.unreadNotifications ? 2 * mm : 1.5 * mm
            height: width
            radius: height / 2
        }
    }
*/
}
