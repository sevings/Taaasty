import QtQuick 2.7
import org.binque.taaasty 1.0

Rectangle {
    id: back
    color: window.backgroundColor
    clip: true
    height: visible ? parent.height : 0
    y: window.height
    z: 5
    visible: y < window.height
    state: "closed"
    states: [
        State {
            name: "opened"
            AnchorChanges {
                target: back
                anchors.top: parent.top
            }
        },
        State {
            name: "closed"
            AnchorChanges {
                target: back
                anchors.top: parent.bottom
            }
        }
    ]
    transitions: [
        Transition {
            from: "opened"
            to: "closed"
            AnchorAnimation {
                duration: 300
            }
        },
        Transition {
            from: "closed"
            to: "opened"
            AnchorAnimation {
                duration: 300
            }
        }
    ]
    MyListView {
        id: notifsView
        anchors.fill: parent
        model: NotifsModel
        spacing: 2 * mm
        cacheBuffer: back.visible ? 2 * window.height : 0
        delegate: MouseArea {
            id: notif
            width: window.width
            readonly property int h: notifName.paintedHeight + notifText.paintedHeight
            height: (h > notifAvatar.height ? h : notifAvatar.height) + 2 * mm
            onClicked: {
                if (notification.entityType === 'Entry')
                {
                    if (!notification.entityId)
                        return;

                    window.hideNotifs();
                    window.pushFullEntryById(notification.entityId);
                }
                else if (notification.entityType === 'Relationship')
                {
                    window.hideNotifs();
                    window.pushTlog(notification.sender.id);
                }
                else if (notification.entityType === 'Comment')
                {
                    if (!notification.parentId)
                        return;

                    window.hideNotifs();
                    window.pushFullEntryById(notification.parentId);
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
                    enabled: notification.parentType !== 'AnonymousEntry'
                    onClicked: {
                        if (back.y > 0)
                            return;

                        window.hideNotifs();
                        window.pushProfileById(notification.sender.id);
                    }
                }
            }
            Text {
                id: notifName
                text: '<b>' + notification.sender.name + '</b> ' 
                        + notification.actionText
                color: window.textColor
                anchors {
                    top: notifAvatar.top
                    left: notifAvatar.right
                    right: unreadNotice.left
                    leftMargin: 1 * mm
                    rightMargin: 1 * mm
                }
                wrapMode: Text.Wrap
                font.pointSize: window.fontSmaller
                style: Text.Raised
                styleColor: 'lightblue'
            }
            Rectangle {
                id: unreadNotice
                anchors {
                    verticalCenter: notifAvatar.verticalCenter
                    right: parent.right
                    margins: 2 * mm
                }
                width: 1.5 * mm
                height: width
                radius: height / 2
                color: window.greenColor
                visible: !notification.read
            }
            ThemedText {
                id: notifText
                text: notification.text
                anchors {
                    top: notifName.bottom
                    left: notifName.left
                    right: notifName.right
                }
                font.pointSize: window.fontSmaller
            }
        }
    }
}
