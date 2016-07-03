import QtQuick 2.7

Rectangle {
    color: window.backgroundColor
    signal popped
    onPopped: window.popFromStack()
    property bool poppable
}
