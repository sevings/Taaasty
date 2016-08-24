import QtQuick 2.7

MouseArea {
    property Pane body
    signal popped
    anchors.fill: parent
    drag.target: body
    drag.axis: Drag.XAxis
    drag.minimumX: 0
    drag.maximumX: body ? body.width * 0.9 : 0
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

        if (!body)
            return;

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
    onClicked: {
        if (!body)
            return;

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
}
