import QtQuick 2.3
import org.binque.taaasty 1.0

Rectangle {
    id: back
    color: window.backgroundColor
    Poppable {
        body: back
    }
    MyListView {
        id: notifsView
        anchors.fill: parent
        property NotificationsModel notifs: NotificationsModel { }
        model:  notifs
        spacing: 20
        delegate: MouseArea {
            id: notif
            anchors.left: parent.left
            anchors.right: parent.right
            height: notifName.paintedHeight + notifText.paintedHeight + 20
//            body: back
            onClicked: {
                // Ctrl.goToNotificationSource(notifsModel.get(notifsView.indexAt(ix, iy)));
            }
            SmallAvatar {
                id: notifAvatar
                anchors.margins: 10
                source: notification.sender.thumb64
                symbol: notification.sender.symbol
            }
            Text {
                id: notifName
                text: '<b>' + notification.sender.name + '</b> ' 
                        + notification.actionText
                color: window.textColor
                anchors.top: parent.top
                anchors.left: notifAvatar.right
                anchors.right: unreadNotice.left
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                wrapMode: Text.Wrap
                font.pointSize: window.fontSmaller
                style: Text.Raised
                styleColor: 'lightblue'
            }
            Rectangle {
                id: unreadNotice
                anchors.verticalCenter: notifName.verticalCenter
                anchors.right: parent.right
                anchors.margins: 20
                width: 15
                height: width
                radius: height / 2
                color: window.brightColor
                visible: !notification.read
            }
            Text {
                id: notifText
                text: notification.text
                color: window.textColor
                anchors.rightMargin: 10
                anchors.top: notifName.bottom
                anchors.left: notifName.left
                anchors.right: parent.right
                wrapMode: Text.Wrap
                font.pointSize: window.fontSmaller
            }
        }
    }
}
