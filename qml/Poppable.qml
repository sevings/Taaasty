import QtQuick 2.7

MouseArea {
    property Item body
    signal popped
    anchors.fill: parent
    drag.target: body
    drag.axis: Drag.XAxis
    drag.minimumX: 0
    drag.maximumX: body.width * 0.9
    drag.threshold: 2 * mm
//    propagateComposedEvents: true
    enabled: false
    Component.onCompleted: {
        enabled = true;
    }
    onReleased: {
        if (body.poppable)
        {
            if (body.x > body.width / 3)
                body.popped();
            else
                backAnimation.start();
        }
        else
        {
            if (body.x > body.width / 3)
                forwardAnimation.start();
            else
                backAnimation.start();
        }
    }
    onPositionChanged: {
        if (!drag.active)
            return;

        body.opacity = 1 - body.x / body.width;
    }
    onClicked: {
        if (body.x <= 0)
            mouse.accepted = false;
        else {
            backAnimation.start();
            mouse.accepted = true;
        }
    }
    onDoubleClicked: {
        // supress second click
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
    ParallelAnimation {
        id: forwardAnimation
        PropertyAnimation {
            target: body
            property: "x"
            to: body.width * 0.7
            duration: 100
        }
        PropertyAnimation {
            target: body
            property: "opacity"
            to: 0.3
            duration: 100
        }
    }
}
