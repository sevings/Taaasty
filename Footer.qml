import QtQuick 2.3

Rectangle {
    id: footer
    anchors.left: parent.left
    anchors.right: parent.right
    height: avatar.height
    y: parent.height
    z: 20
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
        MouseArea {
            anchors.fill: parent
//                    onClicked: back.avatarClicked(entry.tlog, entry.author)
//                enabled: mode !== FeedModel.AnonymousMode
        }
    }
    Text {
        id: title
        text: 'Live'
        color: window.textColor
        font.pointSize: window.fontBigger
        anchors.left: avatar.right
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.margins: 10
        horizontalAlignment: Text.AlignLeft
        elide: Text.ElideRight
    }
    MouseArea {
        id: notifButton
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: parent.height * 1.5
        onClicked: {
            window.toggleNotifs()
        }
        Rectangle {
            id: notifNotice
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.top
            color: window.unreadNotifications ? window.brightColor : '#575757'
            width: 15
            height: width
            radius: height / 2
        }
    }
}
