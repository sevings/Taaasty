import QtQuick 2.3

Rectangle {
    id: back
    anchors.left: parent.left
    anchors.right: parent.right
    height: 4 * mm + label.height + inputRect.height + acceptButton.height
    color: window.backgroundColor
    border.color: window.secondaryTextColor
    border.width: 0.2 * mm
    radius: 0.8 * mm
    property string mode: 'tlog'
    property alias text: input.text
    property alias echoMode: input.echoMode
    property alias what: label.text
    property alias buttonText: acceptButton.text
    property alias inputMethodHints: input.inputMethodHints
    function clear() {
        input.text = '';
    }
    onFocusChanged: {
        if (focus)
            input.focus = true;
    }
    signal accepted
    state: "closed"
    states: [
        State {
            name: "opened"
            AnchorChanges {
                target: back
                anchors.top: undefined
                anchors.bottom: parent.bottom
            }
        },
        State {
            name: "closed"
            AnchorChanges {
                target: back
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
        visible: parent.state === "opened"
        onClicked: window.hideLineInput()
    }
    Text {
        id: label
        text: {
            if (mode === 'tlog')
                'Тлог или поток';
            else if (mode === 'rating')
                'Минимальный рейтинг';
        }
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 1 * mm
        wrapMode: Text.Wrap
        font.pointSize: window.fontNormal
        color: window.textColor
        height: contentHeight
    }
    Rectangle {
        id: inputRect
        anchors.top: label.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 1 * mm
        gradient: Gradient {
            GradientStop { position: 0; color: 'black' }
            GradientStop { position: 1; color: '#292929' }
        }
        border.color: window.secondaryTextColor
        border.width: 0.1 * mm
        radius: 0.8 * mm
        height: input.contentHeight + 2 * mm
        TextInput {
            id: input
            anchors.fill: parent
            anchors.margins: 1 * mm
            color: window.textColor
            font.pointSize: window.fontNormal
            onAccepted: back.accepted()
            inputMethodHints: {
                if (mode === 'tlog')
                    Qt.ImhNoPredictiveText | Qt.ImhPreferLowercase;
                else if (mode === 'rating')
                    Qt.ImhDigitsOnly;
            }
        }
    }
    ThemedButton {
        id: acceptButton
        anchors.top: inputRect.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        width: window.width / 5
        text: {
            if (mode === 'tlog')
                'Перейти';
            else if (mode === 'rating')
                'Показать';
        }
        enabled: input.text
        onClicked: parent.accepted()
    }
}
