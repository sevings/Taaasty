import QtQuick 2.7
import QtQuick.Templates 2.0

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
        color: window.textColor
        font.pointSize: window.fontNormal
        wrapMode: TextEdit.Wrap
        textFormat: Text.PlainText
        padding: 1 * mm
        implicitHeight: contentHeight + 2 * mm
        implicitWidth: 30 * mm
//        selectByMouse: true
        background: Rectangle {
            gradient: Gradient {
                GradientStop { position: 0; color: 'black' }
                GradientStop { position: 1; color: '#181818' }
            }
            border.color: window.textColor
            border.width: 0.2 * mm
            radius: 0.8 * mm
        }
    }

}
