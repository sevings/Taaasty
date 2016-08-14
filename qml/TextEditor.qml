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
