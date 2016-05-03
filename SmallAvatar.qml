import QtQuick 2.3

MyImage {
    id: avatar
    anchors.top: parent.top
    anchors.left: parent.left
    //anchors.margins: 1 * mm
    width: 8 * mm
    height: 8 * mm
    property string symbol: '?'
    onAvailable: {
        letter.visible = false;
    }
    Text {
        id: letter
        color: window.textColor
        font.pixelSize: window.fontBiggest
        anchors.centerIn: parent
        text: parent.symbol.toUpperCase()
    }
}
