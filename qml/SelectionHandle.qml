import QtQuick 2.7
import QtQuick.Controls.Material 2.0

MouseArea {
    id: handle
    z: 21
    property TextEdit textEdit
    property int textPosition
    signal moved(int currentPosition)
    parent: textEdit
    width: 48 * sp
    height: 60 * sp
    visible: textEdit.hasSelection
    drag.target: handle
    drag.smoothed: false
    onXChanged: {
        if (pressed)
            move();
    }
    onYChanged: {
        if (pressed)
            move();
    }
    onReleased: {
        setPosition();
    }
    function move() {
        textEdit.canCopyPaste = true;
        var currentPosition = textEdit.positionAt(x + width / 2, y);
        moved(currentPosition);
    }
    function setPosition() {
        var positionRectangle = textEdit.positionToRectangle(textPosition)
        x = positionRectangle.x - width / 2
        y = positionRectangle.y;
    }
    Rectangle {
        anchors.horizontalCenter: parent.horizontalCenter
        width: 0.5 * mm
        height: parent.height
        radius: width / 2
        color: Material.primary
        Rectangle {
            width: 2 * mm
            height: width
            radius: width / 2
            color: parent.color
            anchors {
                bottom: parent.bottom
                horizontalCenter: parent.horizontalCenter
            }
        }
    }
}
