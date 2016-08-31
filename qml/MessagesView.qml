import QtQuick 2.7
import QtQuick.Controls.Material 2.0
import org.binque.taaasty 1.0

Pane {
    id: back
    innerFlick: listView
    color: window.darkTheme ? window.backgroundColor : '#e6f1f2'
    property Chat chat: Chat { }
    property Tlog tlog: Tlog {
        tlogId: chat.recipientId
    }
    readonly property bool isMessagesView: true
    signal addGreeting(string slug)
    Component.onCompleted: {
        chat.messages.check();
        listView.positionViewAtEnd();
    }
    Poppable {
        body: back
    }
    Splash {
        visible: !listView.visible
        running: listView.model.hasMore
        text: 'Нет сообщений'
    }
    MyListView {
        id: listView
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: contentHeight > parent.height ? parent.height : contentHeight
        visible: count > 0 || !model.hasMore
        model: chat.messages
        onCountChanged: {
            if (count < 30)
                positionViewAtEnd();
        }
        header: ListBusyIndicator {
            running: listView.model.loading
            visible: listView.model.hasMore
            footer: false
        }
        delegate: Item {
            width: window.width
            height: (messageAvatar.height > messageBack.height
                     ? messageAvatar.height : messageBack.height) + 2 * mm
            readonly property bool isMyMessage: chat.userId === message.userId 
            Poppable {
                body: back
                onClicked: {
                    menu.show(message);
                }
            }
            Component.onCompleted: {
                if (!message.isRead)
                    message.read();

                if (index < 10)
                    listView.model.loadMore();
            }
            SmallAvatar {
                id: messageAvatar
                anchors {
                    top: parent.top
                    margins: 1 * mm
                    left: isMyMessage ? undefined : parent.left
                    right: isMyMessage ? parent.right : undefined
                }
                width: 7 * mm
                user: message.user
                popBody: back
                onClicked: {
                    if (!chat.isAnonymous)
                        window.pushProfileById(message.user.id);
                }
            }
            Rectangle {
                id: messageBack
                anchors {
                    top: parent.top
                    left: isMyMessage ? undefined : messageAvatar.right
                    right: isMyMessage ? messageAvatar.left : undefined
                    margins: 1 * mm
                    leftMargin: isMyMessage ? 1 * mm : 0
                    rightMargin: isMyMessage ? 0 : 1 * mm
                }                
                readonly property int maxWidth: window.width - messageAvatar.width - 2 * mm
                readonly property int textWidth: (messageText.contentWidth > messageDate.contentWidth
                                                  ? messageText.contentWidth : messageDate.contentWidth)
                                                  + 2 * mm
                width: textWidth > maxWidth || messageImages.visible ? maxWidth : textWidth
                height: messageDate.y + messageDate.contentHeight + 1 * mm
                color: window.darkTheme ? '#404040' : window.backgroundColor
                ListView {
                    id: messageImages
                    anchors {
                        top: parent.top
                        left: parent.left
                        right: parent.right
                        margins: 1 * mm
                    }
                    interactive: false
                    spacing: 1 * mm
                    property AttachedImagesModel imagesModel: message.attachedImagesModel
                    height: visible ? (imagesModel.listRatio() * width
                            + (count - 1) * mm) : -1 * mm
                    model: imagesModel
                    visible: imagesModel && count > 0
                    delegate: MyImage {
                        id: picture
                        width: messageImages.width
                        height: image.height / image.width * width
                        url: image.url
                        extension: image.type
                        savable: true
                        popBody: back
                    }
                }
                ThemedText {
                    id: messageText
                    anchors {
                        top: messageImages.bottom
                        left: parent.left
                    }
                    width: messageBack.maxWidth - 2 * mm
                    font.pointSize: window.fontSmaller
                    text: message.text
                    textFormat: message.containsImage ? Text.RichText : Text.StyledText
                    onLinkActivated: window.openLink(link)
                    height: message.text.length > 0 ? contentHeight : -1 * mm
                }
                ThemedText {
                    id: messageDate
                    anchors {
                        top: messageText.bottom
                        left: parent.left
                    }
                    width: messageBack.maxWidth - 2 * mm
                    font.pointSize: window.fontSmallest
                    text: (chat.type == Chat.PrivateConversation && message.user.name.length
                           ? '' : message.user.name + ', ')
                          + message.createdAt
                    color: window.secondaryTextColor
                }
            }
            Rectangle {
                id: unreadMessage
                anchors {
                    verticalCenter: messageBack.top
                    horizontalCenter: messageBack.right
                }
                width: 1.5 * mm
                height: width
                radius: height / 2
                color: Material.primary
                visible: !message.isRead
            }
        }
        footer: MessageEditor {
            id: messageEditor
            onSent: {
                chat.sendMessage(messageEditor.message)
            }
            visible: chat.canTalk
            height: visible ? implicitHeight : - 1 * mm
            Connections {
                target: chat
                onMessageSent: {
                    messageEditor.clear();
                }
            }
            Connections {
                target: back
                onAddGreeting: {
                    messageEditor.addGreeting(slug);
                }
            }
        }
    }
    Popup {
        id: menu
        height: menuColumn.height + 2 * mm
        anchors.margins: 2 * mm
        property Message message: Message { }
        function close() {
            state = "closed";
        }
        function show(msg) {
            menu.message = msg;
            window.hideFooter();
            state = "opened";
        }
        onClosing: menu.close()
        Column {
            id: menuColumn
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                topMargin: 1 * mm
                bottomMargin: 1 * mm
            }
            spacing: 1 * mm
            MenuItem {
                text: 'Ответить'
                onTriggered: {
                    addGreeting(menu.message.user.slug);
                    menu.close();
                    listView.positionViewAtEnd();
                }
            }
        }
    }
}
