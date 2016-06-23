import QtQuick 2.7

Rectangle {
    id: back
    color: window.backgroundColor
    signal popped
    onPopped: window.popFromStack()
    property bool poppable
    property bool fullLoad: true
    Poppable {
        body: back
    }
    ThemedProgressBar {
        id: tlogBar
        anchors.bottom: allBar.visible ? parent.verticalCenter : undefined
        anchors.verticalCenter: allBar.visible ? undefined : parent.verticalCenter
        anchors.left: parent.left
        anchors.right: parent.right
        text: 'Тлог'
        value: 0
        to: 0
    }
    ThemedProgressBar {
        id: allBar
        visible: back.fullLoad
        anchors.top: parent.verticalCenter
        anchors.left: parent.left
        anchors.right: parent.right
        text: 'Всего'
        value: 0
        to: 0
        percents: true
    }
}
