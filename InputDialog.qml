import QtQuick 2.3

Popup {
    id: back
    height: 4 * mm + label.height + input.height + acceptButton.height
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
    ThemedText {
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
        height: contentHeight
    }
    LineInput {
        id: input
        anchors.top: label.bottom
        anchors.left: parent.left
        anchors.right: parent.right
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
    ThemedButton {
        id: acceptButton
        anchors.top: input.bottom
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
