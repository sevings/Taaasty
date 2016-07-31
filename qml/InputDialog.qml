import QtQuick 2.7

Popup {
    id: back
    height: 4 * mm + label.height + input.height + acceptButton.height
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
    onClosing: window.hideLineInput()
    ThemedText {
        id: label
        text: {
            if (mode === 'tlog')
                'Тлог или поток';
            else if (mode === 'rating')
                'Минимальный рейтинг';
            else if (mode === 'query')
                'Поиск здесь';
            else if (mode === 'save')
                'Название изображения';
        }
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: contentHeight
    }
    LineInput {
        id: input
        anchors {
            top: label.bottom
            left: parent.left
            right: parent.right
        }
        onAccepted: {
            if (acceptButton.enabled)
                window.handleInputLine(mode, input.text);
        }
        inputMethodHints: {
            if (mode === 'tlog')
                Qt.ImhNoPredictiveText | Qt.ImhPreferLowercase;
            else if (mode === 'rating')
                Qt.ImhDigitsOnly;
            else if (mode === 'query' || mode === 'save')
                Qt.ImhNone;
        }
    }
    ThemedButton {
        id: acceptButton
        anchors {
            top: input.bottom
            right: parent.right
            left: parent.left
        }
        width: window.width / 5
        text: {
            if (mode === 'tlog')
                'Перейти';
            else if (mode === 'rating')
                'Показать';
            else if (mode === 'query')
                'Искать';
            else if (mode === 'save')
                'Сохранить';
        }
        enabled: input.text
        onClicked: window.handleInputLine(mode, input.text)
    }
}
