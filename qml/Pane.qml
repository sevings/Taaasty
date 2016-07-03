import QtQuick 2.0

Rectangle {
    color: window.backgroundColor
    signal popped
    onPopped: window.popFromStack()
    property bool poppable
}
