import QtQuick 2.7

Item {
    id: editor
    anchors.left: parent.left
    anchors.right: parent.right
    implicitHeight: input.height + 2 * mm
    property alias message: input.text
    signal sent
    onFocusChanged: if (focus) input.focus = true
    function clear() {
        input.clear();
    }
    function addGreeting(slug) {
        input.addGreeting(slug);
    }
    TextEditor {
        id: input
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: button.left
        height: 12 * mm
    }
    ThemedButton {
        id: button
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        width: parent.width / 5
        text: '+'
        enabled: input.text
        onClicked: {
            editor.sent();
        }
    }
}
