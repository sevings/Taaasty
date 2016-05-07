import QtQuick 2.3
import org.binque.taaasty 1.0

Rectangle {
    id: back
    color: window.backgroundColor
    clip: true
    signal avatarClicked(int tlog)
    signal entryRequested(int entry, bool showProfile)
    signal tlogRequested(int tlogId)
    property NotificationsModel notifs: NotificationsModel { }
    Poppable {
        body: back
    }
    MyListView {
        id: notifsView
        anchors.fill: parent
        model:  notifs
        spacing: 2 * mm
        delegate: MouseArea {
            id: notif
//            anchors.left: parent.left
//            anchors.right: parent.right
            width: window.width
            readonly property int h: notifName.paintedHeight + notifText.paintedHeight
            height: (h > notifAvatar.height ? h : notifAvatar.height) + 2 * mm
//            body: back
            readonly property bool showProfile: notification.parentType !== 'AnonymousEntry'
            onClicked: {
                if (notification.entityType === 'Entry')
                {
                    if (!notification.entityId)
                        return;

                    window.hideNotifs();
                    entryRequested(notification.entityId, showProfile);
                }
                else if (notification.entityType === 'Comment')
                {
                    if (!notification.parentId)
                        return;

                    window.hideNotifs();
                    entryRequested(notification.parentId, showProfile);
                }
                else if (notification.entityType === 'Relationship')
                {
                    if (!notification.parentId)
                        return;

                    window.hideNotifs();
                    tlogRequested(notification.sender.id);
                }
                else
                    console.log(notification.entityType);
            }
            SmallAvatar {
                id: notifAvatar
                anchors.margins: 1 * mm
                user: notification.sender
                MouseArea {
                    anchors.fill: parent
                    enabled: showProfile
                    onClicked: {
                        window.hideNotifs();
                        avatarClicked(notification.sender.id);
                    }
                }
            }
            Text {
                id: notifName
                text: '<b>' + notification.sender.name + '</b> ' 
                        + notification.actionText
                color: window.textColor
                anchors.top: notifAvatar.top
                anchors.left: notifAvatar.right
                anchors.right: unreadNotice.left
                anchors.leftMargin: 1 * mm
                anchors.rightMargin: 1 * mm
                wrapMode: Text.Wrap
                font.pointSize: window.fontSmaller
                style: Text.Raised
                styleColor: 'lightblue'
            }
            Rectangle {
                id: unreadNotice
                anchors.verticalCenter: notifAvatar.verticalCenter
                anchors.right: parent.right
                anchors.margins: 2 * mm
                width: 1.5 * mm
                height: width
                radius: height / 2
                color: window.brightColor
                visible: !notification.read
            }
            Text {
                id: notifText
                text: notification.text
                color: window.textColor
                anchors.topMargin: 1 * mm
                anchors.top: notifName.bottom
                anchors.left: notifName.left
                anchors.right: notifName.right
                wrapMode: Text.Wrap
                font.pointSize: window.fontSmaller
            }
        }
    }
}
