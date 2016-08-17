import QtQuick 2.0
import org.binque.taaasty 1.0

MyImage {
    id: link
    property Media media
    width: window.width
    height: media ? media.thumbnail.height / media.thumbnail.width * width : -anchors.topMargin
    url: media ? media.thumbnail.url : ''
    extension: media ? media.thumbnail.type : ''
    function play() {
        Qt.openUrlExternally(media.url)
    }
    ThemedText {
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            margins: 0.5 * mm
        }
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: window.fontSmaller
        style: Text.Outline
        styleColor: window.backgroundColor
        text: media ? media.title : ''
    }
}
