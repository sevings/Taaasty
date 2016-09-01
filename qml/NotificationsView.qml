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
            readonly property int h: notifName.paintedHeight + notifText.paintedHeight + 1 * mm
            height: (h > notifAvatar.height ? h : notifAvatar.height) + 2 * mm
            onClicked: {
                var fullEntry = model.notification.entry;
                if (fullEntry)
                {
                    back.hide();
                    window.pushFullEntry(fullEntry);
                }
                else if (model.notification.entityType == 3)//Notification.RelationshipType)
                {
                    back.hide();
                    window.pushTlog(model.notification.sender.id);
                }
            }
            Rectangle {
                anchors.fill: parent
                color: Material.primary
                visible: notif.pressed
            }
            SmallAvatar {
                id: notifAvatar
                anchors.margins: 1 * mm
                user: model.notification.sender
                acceptClick: back.y <= 0 && model.notification.parentType !== 'AnonymousEntry'
                onClicked: {
                    back.hide();
                    window.pushProfileById(model.notification.sender.id);
                }
            }
            Q.Label {
                id: notifName
                text: '<b>' + model.notification.sender.name + '</b> '
                        + model.notification.actionText
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
                visible: !model.notification.read
            }
            ThemedText {
                id: notifText
                text: model.notification.text
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
