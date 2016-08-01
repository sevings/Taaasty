import QtQuick 2.7
import QtQuick.Controls 2.0 as Q
import QtGraphicalEffects 1.0

Item {
    property bool poppable
    signal popped
    onPopped: window.popFromStack()
    Q.Pane {
        id: pane
        anchors.fill: parent
        padding: 0
    }
    DropShadow {
        anchors.fill: pane
        horizontalOffset: - 0.5 * mm
        samples: 11
        color: "#80000000"
        source: pane
        cached: true
        visible: parent.x > 0
    }
}
