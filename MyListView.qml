import QtQuick 2.3
import org.binque.taaasty 1.0

ListView {
    id: list
    highlightFollowsCurrentItem: false
    cacheBuffer: 1000
    flickableDirection: Flickable.VerticalFlick
    property bool poppable: true
    boundsBehavior: Flickable.DragOverBounds
    onContentYChanged: {
        if (!dragging)
            return;

        if (verticalVelocity > 0)
            window.hideFooter();
        else
            window.showFooter();
    }
//    add: Transition {
//        NumberAnimation { property: "opacity"; from: 0;   to: 1.0; duration: 300 }
//        NumberAnimation { property: "scale";   from: 0.8; to: 1.0; duration: 300 }
//    }
//    remove: Transition {
//        NumberAnimation { property: "opacity"; from: 1.0; to: 0; duration: 300 }
//        NumberAnimation { property: "scale";   from: 1.0; to: 0.8; duration: 300 }
//    }
//    displaced: Transition {
//        NumberAnimation { property: "y"; duration: 300 }
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
