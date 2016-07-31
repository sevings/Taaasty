import QtQuick 2.7
import QtQuick.Controls 2.0 as Q
import QtQuick.Controls.Material 2.0

Q.Pane {
    signal popped
    onPopped: window.popFromStack()
    property bool poppable
    padding: 0
}
