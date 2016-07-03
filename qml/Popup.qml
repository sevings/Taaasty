import QtQuick 2.7

Rectangle {
    id: popup
    anchors.left: parent.left
    anchors.right: parent.right
    y: 0
    z: 20
    visible: y < window.height || !closable
    enabled: visible
    property bool closable: true
    signal closing
    gradient: Gradient {
        GradientStop { position: 0; color: window.darkTheme ? '#373737' : '#adbac6' }
        GradientStop { position: 1; color: window.darkTheme ? '#000000' : '#42515a' }
    }
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
                anchors.top: undefined
                anchors.bottom: parent.bottom
            }
        },
        State {
            name: "closed"
            AnchorChanges {
                target: popup
                anchors.top: parent.bottom
                anchors.bottom: undefined
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
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.top
        height: parent.y
        visible: closable && parent.visible
        onClicked: closing()
    }
    MouseArea {
        anchors.fill: parent
        visible: closable
    }
}
