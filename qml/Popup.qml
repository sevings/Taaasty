import QtQuick 2.7
import QtQuick.Controls 2.0 as Q
import QtQuick.Controls.Material 2.0

Rectangle {
    id: popup
    anchors {
        left: parent.left
        right: parent.right
    }
    y: 0
    z: 20
    visible: y < window.height || !closable
    enabled: visible
    property bool closable: true
    signal closing
    color: Material.primary
    onVisibleChanged: {
        if (!closable)
            return;

        if (visible) {
            enabled = true;
            popup.forceActiveFocus();
        }
        else
            enabled = false;
    }
    Keys.onBackPressed: {
        if (!closable)
            return;

        closing();
        event.accepted = true;
    }
    state: "closed"
    states: [
        State {
            name: "opened"
            AnchorChanges {
                target: popup
                anchors {
                    top: undefined
                    bottom: parent.bottom
                }
            }
        },
        State {
            name: "closed"
            AnchorChanges {
                target: popup
                anchors {
                    top: parent.bottom
                    bottom: undefined
                }
            }
        }
    ]
    transitions: [
        Transition {
            from: "opened"
            to: "closed"
            AnchorAnimation {
                duration: 200
            }
        },
        Transition {
            from: "closed"
            to: "opened"
            AnchorAnimation {
                duration: 200
            }
        }
    ]
    MouseArea {
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.top
        }
        height: parent.y
        visible: closable && parent.visible
        onClicked: closing()
    }
    MouseArea {
        anchors.fill: parent
        visible: closable
        anchors.margins: -2 * mm
    }
}
