import QtQuick 2.7

Rectangle {
    id: back
    anchors {
        left: parent.left
        right: parent.right
    }
    height: visible ? parent.height : 0
    color: window.backgroundColor
    clip: true
    z: 20
    visible: y < window.height
    readonly property bool showing: state == "opened"
    function show() {
        state = "opened";
    }
    function hide() {
        state = "closed";
    }
    function toggle() {
        if (showing)
            hide();
        else
            show();
    }
    state: "closed"
    states: [
        State {
            name: "opened"
            AnchorChanges {
                target: back
                anchors.top: parent.top
            }
        },
        State {
            name: "closed"
            AnchorChanges {
                target: back
                anchors.top: parent.bottom
            }
        }
    ]
    transitions: [
        Transition {
            from: "opened"
            to: "closed"
            AnchorAnimation {
                duration: 300
            }
        },
        Transition {
            from: "closed"
            to: "opened"
            AnchorAnimation {
                duration: 300
            }
        }
    ]
}
