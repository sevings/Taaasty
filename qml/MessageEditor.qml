import QtQuick 2.7
import QtQuick.Controls 2.0 as Q

Item {
    id: editor
    anchors {
        left: parent.left
        right: parent.right
    }
    implicitHeight: 16 * mm
    property alias message: input.text
    property bool uploading: false
    signal sent
    function clear() {
        input.clear();
        uploading = false;
    }
    function addGreeting(slug) {
        input.addGreeting(slug);
    }
    TextEditor {
        id: input
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: button.left
        }
        height: 14 * mm
        Q.BusyIndicator {
            id: busy
            height: 5 * mm
            width: 5 * mm
            anchors.centerIn: parent
            running: uploading
        }
    }
    ThemedButton {
        id: button
        anchors {
            bottom: parent.bottom
            right: parent.right
        }
        width: parent.width / 5
        text: '+'
        enabled: !editor.uploading && input.text
        onClicked: {
            editor.uploading = true;
            editor.sent();
        }
    }
}
