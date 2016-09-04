import QtQuick 2.7
import TextReader 1.0

Pane {
    id: back
    innerFlick: flick
    MyFlickable {
        id: flick
        anchors.fill: parent
        contentHeight: item.height
//        interactive: back.x == 0
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
                text: Qt.application.name + ' ' + Qt.application.version
                      + reader.read()
            }
            TextReader {
                id: reader
                source: ':/other/about.html'
            }
        }
    }
}
