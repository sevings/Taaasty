import QtQuick 2.3
import org.binque.taaasty 1.0

MyImage {
    id: avatar
    anchors.top: parent.top
    anchors.left: parent.left
    //anchors.margins: 1 * mm
    width: 8 * mm
    height: 8 * mm
    property User user: User { }
    property string symbol: user && user.name ? user.name[0].toUpperCase() : '?'
    property string defaultSource: ''
    url: user && (dp < 2 ? user.thumb64 : user.thumb128) || defaultSource
    backgroundColor: user && user.backgroundColor || '#373737'
    onAvailable: {
        letter.visible = false;
    }
    Text {
        id: letter
        color: user && user.nameColor || window.textColor
        font.pixelSize: avatar.width / 2
        anchors.centerIn: parent
        text: parent.symbol.toUpperCase()
    }
}
