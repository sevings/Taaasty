import QtQuick 2.7
import org.binque.taaasty 1.0

Pane {
    id: back
    readonly property bool customTitle: true
    readonly property string title: 'Сообщения'
    Poppable {
        body: back
    }
    Splash {
        visible: !listView.visible
        text: listView.model.hasMore ? 'Загрузка…' : 'Нет бесед'
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
            readonly property int textHeight: lastMessage.y + lastMessage.height - 1 * mm
            height: (textHeight > chatAvatar.height ? textHeight : chatAvatar.height) + 2 * mm
            Poppable {
                body: back
                onClicked: {
                    if (chat.unreadCount > 0)
                        chat.readAll();

                    if (chat.entry) {
                        if (!chat.entry.url.length)
                            chat.entry.reload();
                        window.pushFullEntry(chat.entry);
                    }
                    else
                        window.pushMessages(chat);
                }
            }
            SmallAvatar {
                id: chatAvatar
                anchors {
                    top: parent.top
                    margins: 1 * mm
                }
                user: chat.entry ? chat.entry.author : chat.recipient
                Poppable {
                    body: back
                    onClicked:
                    {
                        if (chat.isAnonymous || (!chat.recipient && !chat.entry))
                            return;

                        window.pushProfileById(chatAvatar.user.id);
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
                text: chat.topic
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
                text: chat.messages.lastMessage.createdAt
                font.pointSize: window.fontSmaller
                color: window.secondaryTextColor
                elide: Text.AlignRight
                wrapMode: Text.NoWrap
            }
            SmallAvatar {
                id: lastMessageAvatar
                anchors {
                    top: chatNick.bottom
                    left: chatAvatar.right
                    margins: 1 * mm
                }
                user: visible ? chat.author(chat.messages.lastMessage.userId)
                              : chatAvatar.user
                width: 4 * mm
                height: 4 * mm
                visible: !chat.isAnonymous && chat.messages.lastMessage.userId !== chat.recipientId
                         && (chat.entry ? chat.messages.lastMessage.userId !== chat.entry.author.id : true)
            }
            ThemedText {
                id: lastMessage
                anchors {
                    top: chatNick.bottom
                    left: lastMessageAvatar.visible ? lastMessageAvatar.right : chatAvatar.right
                    right: unreadMessages.visible ? unreadMessages.left : parent.right
                }
                font.pointSize: window.fontSmallest
                color: window.secondaryTextColor
                text: chat.messages.lastMessage.text
                elide: Text.ElideRight
                wrapMode: Text.NoWrap
//                textFormat: Text.RichText
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
                visible: !chat.messages.lastMessage.isRead
            }
        }
    }
}
