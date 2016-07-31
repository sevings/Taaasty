import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0

Flickable {
    id: flickText
    anchors.margins: 1 * mm
    flickableDirection: Flickable.VerticalFlick
    property alias text: input.text
    onFocusChanged: if (focus) {
                        input.forceActiveFocus();
                    }
    function addGreeting(slug) {
        input.insert(0, '@' + slug + ', ');
        input.forceActiveFocus();
    }
    function insertTags(opening, closing) {
        if (!input.focus)
            return;

        input.insert(input.cursorPosition, opening);
        input.cursorPosition += opening.length;
        if (closing)
            input.insert(input.cursorPosition, closing);
//        var oldText = input.text.toString();
//        var cursor = input.cursorPosition;
//        var positionY = flickText.contentY;

//        var before = oldText.substring(0, cursor);
//        var after = oldText.substring(cursor);
//        var newText = before + opening + (closing !== undefined ? closing : '') + after;
//        input.text = newText;

//        flickText.contentY = positionY;
//        input.cursorPosition = cursor + opening.length;
    }
    function clear() {
        input.text = '';
    }
    TextArea.flickable: TextArea {
        id: input
        font.pointSize: window.fontNormal
        wrapMode: TextEdit.Wrap
        textFormat: Text.PlainText
        padding: 1 * mm
        implicitHeight: contentHeight + 2 * mm
        implicitWidth: 30 * mm
    }
}
