import QtQuick 2.7

Pane {
    id: back
    Flickable {
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick
        contentHeight: item.height
        interactive: back.x == 0
        Item {
            id: item
            width: window.width
            height: aboutText.contentHeight > window.height
                    ? aboutText.contentHeight : window.height
            Poppable {
                body: back
            }
            ThemedText {
                id: aboutText
                anchors {
                    left: parent.left
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }
                horizontalAlignment: Text.AlignHCenter
                onLinkActivated: window.openLink(link)
                text: Qt.application.name + ' ' + Qt.application.version + '<br><br>'
                      + 'Программа распространяется <b>как&nbsp;есть</b> безо всяких гарантий и условий, '
                      + 'явных и подразумеваемых, в надежде, что она кому-то окажется полезной.<br>'
                      + 'Со всеми вопросами, сообщениями о проблемах, пожеланиями, '
                      + 'а также с благодарностями вы можете <a href="http://taaasty.com/~281926">обращаться к автору.</a><br><br>'
                      + 'Icons made by <a href="http://www.flaticon.com/authors/catalin-fertu">Catalin Fertu</a> '
                      + 'or <a href="http://www.flaticon.com/authors/freepik">Freepik</a> '
                      + 'from www.flaticon.com is licensed under CC BY 3.0.'
            }
        }

    }


}
