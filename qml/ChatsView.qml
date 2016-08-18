import QtQuick 2.7
import QtQuick.Controls 2.0 as Q
import QtQuick.Controls.Material 2.0
import org.binque.taaasty 1.0

Pane {
    id: back
    innerFlick: listView
    readonly property bool customTitle: true
    readonly property string title: 'Сообщения'
    readonly property bool isChatsView: true
    Component.onCompleted: Tasty.reconnectToPusher()
    Poppable {
        body: back
    }
    Splash {
        visible: !listView.visible && !pauseAnimations
        running: listView.model.hasMore
        text: 'Нет бесед'
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
        interactive: back.x == 0
        model: ChatsModel
        delegate: Rectangle {
            width: window.width
            readonly property int textHeight: lastMessage.y + lastMessage.height - 2 * mm
            height: (textHeight > chatAvatar.height ? textHeight : chatAvatar.height) + 4 * mm
            color: pop.pressed ? Material.primary : 'transparent'
            Poppable {
                id: pop
                body: back
                onClicked: {
                    if (chat.unreadCount > 0)
                        chat.readAll();

                    if (chat.entry) {
                        if (!chat.entry.url.length)
                            chat.entry.reload();
                        window.pushFullEntry(chat.entry, true);
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
                    topMargin: 2 * mm
                }
                user: chat.entry ? chat.entry.author : chat.recipient
                paused: pauseAnimations
                popBody: back
                onClicked: {
                    if (chat.isAnonymous || (!chat.recipient && !chat.entry))
                        return;

                    window.pushProfileById(chatAvatar.user.id);
                }
            }
            ThemedText {
                id: chatNick
                anchors {
                    top: parent.top
                    left: chatAvatar.right
                    right: date.left
                    topMargin: 2 * mm
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
                    right: parent.right
                    baseline: chatNick.baseline
                }
                text: chat.lastMessage.createdAt
                font.pointSize: window.fontSmallest
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
                user: visible ? chat.lastMessage.user
                              : chatAvatar.user
                width: 4 * mm
                height: 4 * mm
                visible: !chat.isAnonymous && chat.lastMessage.userId !== chat.recipientId
                         && (chat.entry ? chat.lastMessage.userId !== chat.entry.author.id : true)
                paused: pauseAnimations
                popBody: back
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
                text: chat.lastMessage.truncatedText
                elide: Text.ElideRight
                wrapMode: Text.NoWrap
                textFormat: Text.PlainText
            }
            Rectangle {
                id: unreadMessages
                anchors {
                    verticalCenter: lastMessage.verticalCenter
                    right: parent.right
                    margins: 2 * mm
                }
                width: chat.unreadCount > 0 ? 4 * mm : 1.5 * mm
                height: width
                radius: height / 2
                color: Material.primary
                visible: chat.unreadCount > 0 || chat.isMyLastMessageUnread
                Q.Label {
                    visible: chat.unreadCount > 0
                    anchors.centerIn: parent
                    text: chat.unreadCount
                    font.pointSize: window.fontSmallest
                }
            }
        }
        footer: ListBusyIndicator {
            running: ChatsModel.loading && !pauseAnimations
            visible: ChatsModel.hasMore
        }
    }
}
