import QtQuick 2.7

Text {
    id: loadingText
    z: 100
    anchors {
        verticalCenter: parent.verticalCenter
        left: parent.left
        right: parent.right
    }
    color: window.secondaryTextColor
    horizontalAlignment: Text.AlignHCenter
    font.pointSize: window.fontBigger
    wrapMode: Text.Wrap
    text: 'Загрузка…'
}
