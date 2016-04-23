import QtQuick 2.3

MyImage {
    id: avatar
    anchors.top: parent.top
    anchors.left: parent.left
    //anchors.margins: 10
    width: 64
    height: 64
    property string symbol: '?'
    onAvailable: {
        letter.visible = false;
    }
    Text {
        id: letter
        color: window.textColor
        font.pixelSize: 32
        anchors.centerIn: parent
        text: parent.symbol.toUpperCase()
    }
}
