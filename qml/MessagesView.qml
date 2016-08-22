import QtQuick 2.7
import QtQuick.Controls.Material 2.0
import org.binque.taaasty 1.0

Pane {
    id: back
    innerFlick: listView
    property Chat chat: Chat { }
    property Tlog tlog: Tlog {
        tlogId: chat.recipientId
    }
    readonly property bool customTitle: chat.type !== Chat.PrivateConversation
    readonly property string title: chat.topic
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
        interactive: back.x == 0
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
            height: messageNick.height + messageText.height + messageImages.height + 4 * mm
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
//            SmallAvatar {
//                id: messageAvatar
//                anchors {
//                    top: parent.top
//                    margins: 1 * mm
//                    leftMargin: Settings.userId === message.userId ? 5 * mm : anchors.margins
//                }
//                user: message.user
//                Poppable {
//                    body: back
//                    onClicked:
//                    {
//                        window.pushProfileById(message.user.id);
//                        mouse.accepted = true;
//                    }
//                }
//            }
            ThemedText {
                id: messageNick
                anchors {
                    top: parent.top
                    left: parent.left
//                    right: unreadMessage.visible ? unreadMessage.left : messageDate.left
                    margins: 1 * mm
                    leftMargin: chat.userId === message.userId ? 10 * mm : anchors.margins
                }
                text: message.user.name
                font.pointSize: window.fontSmaller
                elide: Text.ElideRight
                wrapMode: Text.NoWrap
                font.bold: true
                style: Text.Raised
                styleColor: window.greenColor
            }
            ThemedText {
                id: messageDate
                anchors {
                    top: parent.top
                    right: parent.right
                    margins: 1 * mm
                    rightMargin: chat.userId === message.userId ? anchors.margins : 10 * mm
                }
                text: message.createdAt
                font.pointSize: window.fontSmaller
                color: window.secondaryTextColor
                elide: Text.AlignRight
                wrapMode: Text.NoWrap
            }
            Rectangle {
                id: unreadMessage
                anchors {
                    verticalCenter: messageDate.verticalCenter
                    right: messageDate.left
                    margins: 2 * mm
                }
                width: 1.5 * mm
                height: width
                radius: height / 2
                color: Material.primary
                visible: !message.isRead
            }
            ThemedText {
                id: messageText
                anchors {
                    top: messageDate.bottom
                    left: parent.left
                    right: parent.right
                    margins: 1 * mm
                    leftMargin: chat.userId === message.userId ? 10 * mm : anchors.margins
                    rightMargin: chat.userId === message.userId ? anchors.margins : 10 * mm
                }
                font.pointSize: window.fontSmaller
                text: message.text
                textFormat: Text.RichText
                onLinkActivated: window.openLink(link)
                height: message.text.length > 0 ? contentHeight : 0
            }
            ListView {
                id: messageImages
                anchors {
                    top: messageText.bottom
                    left: parent.left
                    right: parent.right
                    margins: 1 * mm
                    leftMargin: messageText.anchors.leftMargin
                    rightMargin: messageText.anchors.rightMargin
                }
                interactive: false
                spacing: 1 * mm
                property AttachedImagesModel imagesModel: message.attachedImagesModel
                height: imagesModel ? (imagesModel.listRatio() * messageText.width
                        + (imagesModel.rowCount() - 1) * mm) : 0
                model: imagesModel
                delegate: MyImage {
                    id: picture
                    width: messageText.width
                    height: image.height / image.width * width
                    url: image.url
                    extension: image.type
                    savable: true
                    popBody: back
                }
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
