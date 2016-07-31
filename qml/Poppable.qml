import QtQuick 2.7

MouseArea {
    property Pane body
    signal popped
    anchors.fill: parent
    drag.target: body
    drag.axis: Drag.XAxis
    drag.minimumX: 0
    drag.maximumX: body.width * 0.9
    drag.threshold: 2 * mm
    property bool shiftable: true
    enabled: false
    Component.onCompleted: {
        enabled = true;
    }
    onReleased: {
        if (!propagateComposedEvents) {
            propagateComposedEvents = true
        }

        if (body.poppable)
        {
            if (body.x > window.width / 2)
                body.popped();
            else
                backAnimation.start();
        }
        else
        {
            if (body.x > 20 * mm && shiftable)
                forwardAnimation.start();
            else
                backAnimation.start();
        }
    }
//    onPositionChanged: {
//        if (!drag.active)
//            return;

//        body.opacity =
//    }
    Binding {
        target: body
        property: 'opacity'
        value: 1 - body.x / body.width
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
//        PropertyAnimation {
//            target: body
//            property: "opacity"
//            to: 1
//            duration: 100
//        }
    }
    ParallelAnimation {
        id: forwardAnimation
        PropertyAnimation {
            target: body
            property: "x"
            to: 40 * mm
            duration: 100
        }
//        PropertyAnimation {
//            target: body
//            property: "opacity"
//            to: 0.3
//            duration: 100
//        }
    }
}
