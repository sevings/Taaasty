import QtQuick 2.3

MouseArea {
    property Item body
    signal popped
    anchors.fill: parent
    drag.target: body
    drag.axis: Drag.XAxis
    drag.minimumX: 0
    drag.maximumX: body.width * 0.9
    propagateComposedEvents: true
    enabled: false
    Component.onCompleted: {
        enabled = true;
    }
    onReleased: {
        if (body.poppable && body.x > body.width / 3)
            body.popped();
        else
            backAnimation.start();
    }
    onPositionChanged: {
        if (!drag.active)
            return;

        body.opacity = 1 - body.x / body.width;
    }
    ParallelAnimation {
        id: backAnimation
        PropertyAnimation {
            target: body
            property: "x"
            to: 0
            duration: 100
        }
        PropertyAnimation {
            target: body
            property: "opacity"
            to: 1
            duration: 100
        }
    }
}
