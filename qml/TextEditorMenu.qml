import QtQuick 2.7
import QtQuick.Controls.Material 2.0
import org.binque.taaasty 1.0

Rectangle {
    id: editMenu
    x: visible ? xCoordinate() : 0
    y: visible ? yCoordinate() : 0
    property Item boundItem
    property int margin: 1.5 * mm
    property TextEditor textEdit
    property Flickable flickable
    property TextHandler handler
    readonly property bool show: (!flickable.movingVertically
                                  && !textEdit.leftSelectionHandle.pressed
                                  && !textEdit.rightSelectionHandle.pressed
                                  && (textEdit.canCopyPaste || (textEdit.hasSelection && textEdit.richEditing)))
    visible: false
    opacity: 0
    width: menuRow.width
    height: menuRow.height
    color: Material.primary
    function xCoordinate() {
        var left   = parent.mapToItem(boundItem, 0, 0).x;
        var start  = textEdit.positionToRectangle(textEdit.selectionStart).left + left;
        var end    = textEdit.positionToRectangle(textEdit.selectionEnd).right  + left;
        var bX     = (end + start - width) / 2;
        var bWidth = boundItem ? boundItem.width : window.width;
        var maxBX  = bWidth - width - margin;
            bX     =  Math.min(Math.max(margin, bX), maxBX);
        return bX - left;
    }
    function yCoordinate() {
        var topMarg = flickable.mapToItem(boundItem, 0, 0).y;

        var top = textEdit.positionToRectangle(textEdit.selectionStart).top
                - flickable.contentY + topMarg;
        if (top > height + margin * 2)
            return top - height - margin - topMarg;

        var bottom = textEdit.positionToRectangle(textEdit.selectionEnd).top
                + textEdit.rightSelectionHandle.height
                - flickable.contentY + topMarg;
        var bHeight = boundItem ? boundItem.height : window.height;
        if (bottom < bHeight - height - margin * 2)
            return bottom + margin - topMarg;

        return bHeight - height - margin - topMarg;
    }
    function hideMenu() {
        state = "hidden";
    }
    function showMenu() {
        state = "shown";
    }
    onShowChanged: {
        if (visible) {
            timer.stop();
            hideMenu();
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
                showMenu();
        }
    }
    state: "hidden"
    states: [
        State {
            name: "shown"
            PropertyChanges {
                target: editMenu
                opacity: 1
                visible: true
            }
        },
        State {
            name: "hidden"
            PropertyChanges {
                target: editMenu
                opacity: 0
                visible: false
            }
        }
    ]
    transitions: [
        Transition {
            from: "hidden"
            to: "shown"
            NumberAnimation {
                target: editMenu
                property: "opacity"
                duration: 200
            }
        }
    ]
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
