import QtQuick 2.7
import QtQuick.Controls 2.0 as Q
import QtQuick.Controls.Material 2.0
import org.binque.taaasty 1.0

PopupFill {
    id: back
    onStateChanged: {
        if (state == "opened")
            Tasty.reconnectToPusher();
        else if (state == "closed")
            NotifsModel.markAsRead();
    }
    Splash {
        visible: !notifsView.visible
        running: notifsView.model.hasMore
        text: 'Нет уведомлений'
    }
    MyListView {
        id: notifsView
        anchors.fill: parent
        visible: count > 0
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

                    back.hide();
                    window.pushFullEntryById(notification.entityId);
                }
                else if (notification.entityType === 'Relationship')
                {
                    back.hide();
                    window.pushTlog(notification.sender.id);
                }
                else if (notification.entityType === 'Comment')
                {
                    if (!notification.parentId)
                        return;

                    back.hide();
                    window.pushFullEntryById(notification.parentId);
                }
                else
                    console.log(notification.entityType);
            }
            Rectangle {
                anchors.fill: parent
                color: Material.primary
                visible: notif.pressed
            }
            SmallAvatar {
                id: notifAvatar
                anchors.margins: 1 * mm
                user: notification.sender
                onClicked: {
                    if (back.y > 0 || notification.parentType === 'AnonymousEntry')
                        return;

                    back.hide();
                    window.pushProfileById(notification.sender.id);
                }
            }
            Q.Label {
                id: notifName
                text: '<b>' + notification.sender.name + '</b> '
                        + notification.actionText
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
                color: Material.primary
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
        footer: ListBusyIndicator {
            running: NotifsModel.loading
            visible: NotifsModel.hasMore
        }
    }
}
