import QtQuick 2.7
import org.binque.taaasty 1.0

Pane {
    id: back
    property Chat chat: Chat { }
    property Tlog tlog: Tlog {
        tlogId: chat.recipientId
    }
    Component.onCompleted: {
        chat.messages.check();
    }
    Poppable {
        body: back
    }
    Splash {
        visible: !listView.visible
        text: listView.model.hasMore ? 'Загрузка…' : 'Нет сообщений'
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
        model: chat.messages
        delegate: Item {
            width: window.width
            readonly property int textHeight: messageText.y + messageText.height + 1 * mm
            height: textHeight + 5 * mm
            Poppable {
                body: back
                onClicked: {

                }
            }
//            SmallAvatar {
//                id: messageAvatar
//                anchors {
//                    top: parent.top
//                    margins: 1 * mm
//                    leftMargin: Settings.userId === message.userId ? 5 * mm : anchors.margins
//                }
//                user: message.author
//                Poppable {
//                    body: back
//                    onClicked:
//                    {
//                        window.pushProfileById(message.author.id);
//                        mouse.accepted = true;
//                    }
//                }
//            }
//            ThemedText {
//                id: messageNick
//                anchors {
//                    top: parent.top
//                    left: messageAvatar.right
//                    margins: 1 * mm
//                    right: unreadMessage.visible ? unreadMessage.left : messageDate.left
//                }
//                text: message.author.name
//                font.pointSize: window.fontSmaller
//                elide: Text.ElideRight
//                wrapMode: Text.NoWrap
//                font.bold: true
//                style: Text.Raised
//                styleColor: window.greenColor
//            }
            ThemedText {
                id: messageDate
                anchors {
                    top: parent.top
                    right: parent.right
                    margins: 1 * mm
                    rightMargin: Settings.userId === message.userId ? anchors.margins : 10 * mm
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
                color: window.greenColor
                visible: !message.read
            }
            ThemedText {
                id: messageText
                anchors {
                    top: messageDate.bottom
                    left: parent.left
                    right: parent.right
                    margins: 1 * mm
                    leftMargin: Settings.userId === message.userId ? 10 * mm : anchors.margins
                    rightMargin: Settings.userId === message.userId ? anchors.margins : 10 * mm
                }
                font.pointSize: window.fontSmaller
                text: message.text
                textFormat: Text.RichText
                onLinkActivated: window.openLink(link)
            }
        }
        footer: MessageEditor {
            id: messageEditor
            onSent: {
                //                    Ctrl.sendMessage(messageEditor.message)
            }
        }
    }
}
