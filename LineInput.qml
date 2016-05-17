import QtQuick 2.5

Rectangle {
//    anchors.top: parent.top
//    anchors.left: parent.left
//    anchors.right: parent.right
//    anchors.margins: 1 * mm
    height: input.height + 1 * mm
    //color: window.backgroundColor
    gradient: Gradient {
        GradientStop { position: 0; color: 'black' }
        GradientStop { position: 1; color: '#292929' }
    }
    border.color: window.secondaryTextColor
    border.width: 0.2 * mm
    radius: 0.8 * mm
    property string line: input.text
    property int echoMode: TextInput.Normal
    function clear() {
        input.text = '';
    }
    onFocusChanged: {
        if (focus)
            input.focus = true;
    }
    signal accepted
    TextInput {
        id: input
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 0.5 * mm
        color: window.textColor
        font.pointSize: window.fontNormal
        onAccepted: parent.accepted()
        echoMode: parent.echoMode
    }
}
