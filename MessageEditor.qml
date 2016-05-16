import QtQuick 2.3

Item {
    id: editor
    anchors.left: parent.left
    anchors.right: parent.right
    height: input.height + 2 * mm
    property string message
    signal sent
    onFocusChanged: if (focus) input.focus = true
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
        text: '+'
        onClicked: {
            editor.message = input.text.toString();
            console.log(editor.message);
            editor.sent();
        }
    }
}
