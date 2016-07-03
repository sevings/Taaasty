import QtQuick 2.7
import org.binque.taaasty 1.0

ListView {
    id: list
    highlightFollowsCurrentItem: false
    cacheBuffer: window.height * 2
    flickableDirection: Flickable.VerticalFlick
    maximumFlickVelocity: 1000 * mm
    interactive: parent.x <= 0
    property bool poppable: true
    boundsBehavior: Flickable.DragOverBounds
//    signal aboveBegin
//    onDraggingVerticallyChanged: {
//        if (!draggingVertically && contentY < -8 * mm)
//            list.aboveBegin();
//    }
    onVerticalVelocityChanged: {
        if (!dragging)
            return;

        if (verticalVelocity > 0)
            window.hideFooter();
        else if (verticalVelocity < 0)
            window.showFooter();
    }
    add: Transition {
        NumberAnimation { property: "opacity"; from: 0;   to: 1.0; duration: 300 }
        NumberAnimation { property: "scale";   from: 0.8; to: 1.0; duration: 300 }
    }
    remove: Transition {
        NumberAnimation { property: "opacity"; from: 1.0; to: 0; duration: 300 }
        NumberAnimation { property: "scale";   from: 1.0; to: 0.8; duration: 300 }
    }
    displaced: Transition {
        NumberAnimation { property: "y"; duration: 300 }
    }
    Rectangle {
        id: scrollbar
        anchors.right: parent.right
        anchors.margins: 0.5 * mm
        y: parent.visibleArea.yPosition * (parent.height - height + h)
        width: 1 * mm
        property int h: parent.visibleArea.heightRatio * parent.height
        height: h > 5 * mm ? h : 5 * mm
        color: window.textColor
        opacity: parent.movingVertically ? 0.7 : 0
        visible: opacity > 0 && height < parent.height * 0.9
        Behavior on opacity {
            NumberAnimation { duration: 500 }
        }
        Behavior on height {
            NumberAnimation { duration: 500 }
        }
        radius: 0.5 * mm
    }
}