import QtQuick 2.3

ListView {
    id: list
    anchors.fill: parent
    anchors.topMargin: 10
    anchors.bottomMargin: 10
    highlightFollowsCurrentItem: false
    cacheBuffer: 1000
    flickableDirection: Flickable.VerticalFlick
//    signal aboveBegin
//    signal nearEnd
//    property int endHeight: 2
//    onContentYChanged: {
//        if (contentHeight - contentY < window.height * endHeight || atYEnd) {
//            list.nearEnd();
//        }
//    }
//    onFlickStarted: {
//        if (contentY < 0) {
//            list.aboveBegin();
//        }
//    }
    Rectangle {
        id: scrollbar
        anchors.right: parent.right
        anchors.margins: 5
        y: parent.visibleArea.yPosition * (parent.height - height + h)
        width: 10
        property int h: parent.visibleArea.heightRatio * parent.height
        height: h > 50 ? h : 50
        color: window.textColor
        opacity: parent.movingVertically ? 0.7 : 0
        visible: height < parent.height * 0.9
        Behavior on opacity {
            NumberAnimation { duration: 500 }
        }
        Behavior on height {
            NumberAnimation { duration: 500 }
        }
        radius: 5
    }
}
