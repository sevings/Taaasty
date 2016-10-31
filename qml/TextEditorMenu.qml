import QtQuick 2.7
import QtQuick.Controls.Material 2.0
import org.binque.taaasty 1.0

Rectangle {
    id: editMenu
    x: visible ? xCoordinate() : 0
    y: visible ? yCoordinate() : 0
    property int spaceAtTop: 0
    property int margin: 1.5 * mm
    property TextEditor textEdit
    property Flickable flickable
    property TextHandler handler
    readonly property bool show: (!flickable.movingVertically
                                  && !textEdit.leftSelectionHandle.pressed
                                  && !textEdit.rightSelectionHandle.pressed
                                  && (textEdit.hasSelection || textEdit.canCopyPaste))
    visible: opacity > 0
    opacity: 0
    width: menuRow.width
    height: menuRow.height
    color: Material.primary
    function xCoordinate() {
        var start  = textEdit.positionToRectangle(textEdit.selectionStart).left;
        var end    = textEdit.positionToRectangle(textEdit.selectionEnd).right;
        var x      = (end + start - width) / 2;
        var maxX   = textEdit.width - width - margin;
        return Math.min(Math.max(margin, x), maxX);
    }
    function yCoordinate() {
        var top = textEdit.positionToRectangle(textEdit.selectionStart).top - flickable.contentY;
        if (top > height + margin * 2 - spaceAtTop)
            return top - height - margin;

        var bottom = textEdit.positionToRectangle(textEdit.selectionEnd).top - flickable.contentY
                + textEdit.rightSelectionHandle.height;
        var bottomMargin = Math.max(margin, textEdit.cursorRectangle.height * 1.5);
        if (bottom < textEdit.height - height - margin - bottomMargin)
            return bottom + margin;

        return textEdit.height - height - bottomMargin;
    }
    Behavior on opacity {
        NumberAnimation {
            duration: 400
        }
    }
    onShowChanged: {
        if (visible) {
            timer.stop();
            opacity = 0;
        }
        else
            timer.start();
    }
    Timer {
        id: timer
        interval: 1000
        repeat: false
        onTriggered: {
            if (show)
                opacity = 1;
        }
    }
    Row {
        id: menuRow
        spacing: 1.5 * mm
        height: undoButton.height
        IconButton {
            id: undoButton
            icon: '../icons/undo-white-128.png'
            enabled: textEdit.canUndo
            visible: textEdit.richEditing && textEdit.hasSelection
            onClicked: {
                textEdit.undo();
                textEdit.forceActiveFocus();
            }
        }
        IconButton {
            icon: '../icons/redo-white-128.png'
            enabled: textEdit.canRedo
            visible: textEdit.richEditing && textEdit.hasSelection
            onClicked: {
                textEdit.redo();
                textEdit.forceActiveFocus();
        }
        }
        VerticalMenuSeparator {
            visible: textEdit.richEditing && textEdit.hasSelection
        }
        IconButton {
            icon: '../icons/cut-white-128.png'
            enabled: textEdit.hasSelection
            visible: textEdit.canCopyPaste && textEdit.hasSelection
            onClicked: {
                textEdit.cut();
                textEdit.forceActiveFocus();
            }
        }
        IconButton {
            icon: '../icons/copy-white-128.png'
            enabled: textEdit.hasSelection
            visible: textEdit.canCopyPaste && textEdit.hasSelection
            onClicked: {
                textEdit.copy();
                textEdit.forceActiveFocus();
            }
        }
        IconButton {
            icon: '../icons/paste-white-128.png'
            enabled: textEdit.canPaste
            visible: textEdit.canCopyPaste
            onClicked: {
                textEdit.paste();
                textEdit.forceActiveFocus();
            }
        }
        IconButton {
            icon: '../icons/all-white-128.png'
            enabled: textEdit.length
            visible: textEdit.canCopyPaste && textEdit.selectedText.length < textEdit.length
            onClicked: {
                textEdit.selectAll();
                textEdit.setHandlePositions();
                textEdit.forceActiveFocus();
            }
        }
        VerticalMenuSeparator {
            visible: textEdit.richEditing && textEdit.hasSelection
        }
        IconButton {
            icon: '../icons/italics-white-128.png'
            checkable: true
            checked: handler.italic
            visible: textEdit.richEditing && textEdit.hasSelection
            onClicked: {
                handler.italic = !handler.italic;
                textEdit.forceActiveFocus();
            }
        }
    }
}
