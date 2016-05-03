import QtQuick 2.3

Item {
    id: editor
    anchors.left: parent.left
    anchors.right: parent.right
    height: input.height + 2 * mm
    property string message
    signal sent
    function clear() {
        input.clear();
    }
    TextEditor {
        id: input
        anchors.top: parent.top
        anchors.right: button.left
        height: button.height
    }
    ThemedButton {
        id: button
        anchors.top: parent.top
        anchors.right: parent.right
        width: parent.width / 5
        //anchors.bottom: parent.bottom
        text: '+'
        onClicked: {
            editor.message = input.text;
//            input.clear();
            editor.sent();
        }
    }
}
