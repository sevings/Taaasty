import QtQuick 2.7
import org.binque.taaasty 1.0

Pane {
    id: back
    Poppable {
        body: back
        Text {
            visible: !listView.visible && !listView.model.hasMore
            anchors.centerIn: parent
            color: window.secondaryTextColor
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: window.fontBigger
            wrapMode: Text.Wrap
            text: 'Нет бесед'
        }
    }
    Splash {
        visible: !listView.visible && listView.model.hasMore
    }
    MyListView {
        id: listView
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: contentHeight > parent.height ? parent.height : contentHeight
        visible: count > 0
        model: ChatsModel
        delegate: Item {
            width: window.width
            height: chatAvatar.height + 2 * mm
            Poppable {
                body: back
                onClicked: {
//                    window.pushFullEntry(entry);
                }
            }
            SmallAvatar {
                id: chatAvatar
                anchors {
                    top: parent.top
                    margins: 1 * mm
                }
                user: chat.recipient
//                Component.onCompleted: {
//                    if (chat.recipient)
//                        user = chat.recipient
//                }
                Poppable {
                    body: back
                    onClicked:
                    {
                        if (chat.recipient)
                            window.pushProfileById(chat.recipient.id);
                        else if (chat.entry)
                            window.pushFullEntryById(chat.entry.entryId);

                        mouse.accepted = true;
                    }
                }
            }
            ThemedText {
                id: chatNick
                anchors {
                    top: parent.top
                    left: chatAvatar.right
                    right: date.left
                }
                text: chat.recipient ? chat.recipient.name : (chat.entry.title || chat.entry.text)
                font.pointSize: window.fontSmaller
                elide: Text.ElideRight
                wrapMode: Text.NoWrap
                horizontalAlignment: Text.AlignLeft
            }
            ThemedText {
                id: date
                anchors {
                    top: parent.top
                    right: parent.right
                }
                text: chat.lastMessage.createdAt
                font.pointSize: window.fontSmaller
                color: window.secondaryTextColor
                elide: Text.AlignRight
                wrapMode: Text.NoWrap
            }
            ThemedText {
                id: lastMessage
                anchors {
                    top: chatNick.bottom
                    left: chatAvatar.right
                    right: unreadMessages.visible ? unreadMessages.left : parent.right
                }
                font.pointSize: window.fontSmallest
                color: window.secondaryTextColor
                text: chat.lastMessage.content
                elide: Text.ElideRight
                wrapMode: Text.NoWrap
            }
            Rectangle {
                id: unreadMessages
                anchors {
                    verticalCenter: lastMessage.verticalCenter
                    right: parent.right
                    margins: 2 * mm
                }
                width: 1.5 * mm
                height: width
                radius: height / 2
                color: window.greenColor
                visible: chat.unreadCount > 0
            }
        }
    }
}
