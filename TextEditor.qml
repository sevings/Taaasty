import QtQuick 2.3

Rectangle {
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.margins: 10
    //color: window.backgroundColor
    gradient: Gradient {
        GradientStop { position: 0; color: 'black' }
        GradientStop { position: 1; color: '#181818' }
    }
    border.color: window.textColor
    border.width: 2
    radius: 8
    clip: true
    property string text: input.text
    function clear() {
        input.text = '';
    }
    function insertTags(opening, closing) {
        if (!input.focus)
            return;
        var oldText = input.text.toString();
        var cursor = input.cursorPosition;
        var positionY = flickText.contentY;
        var before = oldText.substring(0, cursor);
        var after = oldText.substring(cursor);
        var newText = before + opening + (closing !== undefined ? closing : '') + after;
        input.text = newText;
        flickText.contentY = positionY;
        input.cursorPosition = cursor + opening.length;
    }

    Flickable {
        id: flickText
        anchors.fill: parent
        anchors.margins: 5
        contentHeight: input.paintedHeight + 10
        flickableDirection: Flickable.VerticalFlick
        TextEdit {
            id: input
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            color: window.textColor
            font.pointSize: 20
            wrapMode: TextEdit.Wrap
            onCursorRectangleChanged: flickText.ensureVisible(cursorRectangle)
            textFormat: Text.PlainText
        }
        MouseArea {
            anchors.top: input.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: flickText.height - input.height
            visible: height > 0
            onClicked: {
                input.cursorPosition = input.text.length;
                input.focus = true;
            }
        }

        function ensureVisible(r){
            if (contentY >= r.y)
                contentY = r.y;
            else if (contentY + height <= r.y + r.height)
                contentY = r.y + r.height - height;
        }
    }
}

