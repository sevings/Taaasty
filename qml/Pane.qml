import QtQuick 2.7
import QtQuick.Controls 2.0 as Q
import QtGraphicalEffects 1.0

Rectangle {
    id: back
    color: window.backgroundColor
    property Flickable innerFlick
    property bool poppable
    signal popped
    onPopped: window.popFromStack()
    Rectangle {
        id: pane
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
        }
        color: back.color
        width: 3 * mm
    }
    DropShadow {
        enabled: !innerFlick || !innerFlick.movingVertically
        anchors.fill: pane
        horizontalOffset: -2 * mm
        radius: 1 * mm
        samples: 5
        color: "#40000000"
        source: pane
    }
}


