import QtQuick 2.3

Popup {
    id: back
    height: 4 * mm + label.height + inputRect.height + acceptButton.height
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
    onOutsideClicked: window.hideLineInput()
    signal accepted
    Text {
        id: label
        text: {
            if (mode === 'tlog')
                'Тлог или поток';
            else if (mode === 'rating')
                'Минимальный рейтинг';
            else if (mode === 'query')
                'Поиск здесь';
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
                else if (mode === 'query')
                    Qt.ImhNone;
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
            else if (mode === 'query')
                'Искать';
        }
        enabled: input.text
        onClicked: parent.accepted()
    }
}
