import QtQuick 2.7
import QtQuick.Controls 2.0 as Q
import QtQuick.Controls.Material 2.0
import QtGraphicalEffects 1.0

Item {
    id: back
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
        color: window.backgroundColor
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
    Q.Pane {
        anchors.fill: parent
        padding: 0
    }
}


