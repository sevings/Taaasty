import QtQuick 2.3


Rectangle {
    id: back
    anchors.margins: 1 * mm
    gradient: Gradient {
        GradientStop { position: 0; color: 'black' }
        GradientStop { position: 1; color: '#292929' }
    }
    border.color: window.secondaryTextColor
    border.width: 0.1 * mm
    radius: 0.8 * mm
    height: input.contentHeight + 2 * mm
    property alias text: input.text
    property alias inputMethodHints: input.inputMethodHints
    property alias echoMode: input.echoMode
    signal accepted
    onFocusChanged: {
        if (focus)
            input.focus = true;
    }
    TextInput {
        id: input
        anchors.fill: parent
        anchors.margins: 1 * mm
        color: window.textColor
        font.pointSize: window.fontNormal
        onAccepted: back.accepted()
    }
}
