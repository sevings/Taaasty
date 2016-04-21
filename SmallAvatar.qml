import QtQuick 2.3

MyImage {
    id: avatar
    anchors.top: parent.top
    anchors.left: parent.left
    //anchors.margins: 10
    width: 64
    height: 64
    //source: url.length === 0 ? '' : url
    property bool symbol: false
    //property url url
    property string name: '?'
    Text {
        color: window.textColor
        font.pixelSize: 32
        anchors.centerIn: parent
        text: parent.name.length > 3 ? parent.name[0].toUpperCase() : parent.name //user.userpic.symbol
        visible: parent.symbol
    }
}
