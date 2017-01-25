import QtQuick 2.8
import QtQuick.Controls.Material 2.1
import QtQuick.Controls.Material.impl 2.0
import org.binque.taaasty 1.0

Rectangle {
    id: editMenu
    x: visible ? xCoordinate() : 0
    y: 0
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
    layer.enabled: visible
    layer.effect: ElevationEffect {
        elevation: 24
    }
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

        var top    = textEdit.positionToRectangle(textEdit.selectionStart).top  + textEdit.y;
        var bottom = textEdit.positionToRectangle(textEdit.selectionEnd).bottom + textEdit.y;

        if (top > flickable.contentY + flickable.height || bottom < flickable.contentY)
        {
            console.log("outside");
            return false;
        }

        top    += (- flickable.contentY + topMarg);
        bottom += (- flickable.contentY + topMarg
                   + textEdit.rightSelectionHandle.height - textEdit.cursorRectangle.height);

        if (top > height + margin * 2)
            return Math.max(top - height - margin - topMarg, -height - margin);

        var bHeight = boundItem ? boundItem.height : window.height;
        if (bottom < bHeight - height - margin * 2)
            return bottom + margin - topMarg;

        return bHeight - height - margin - topMarg;
    }
    function hideMenu() {
        state = "hidden";
    }
    function showMenu() {
        var y = yCoordinate();
        if (y !== false) {
            editMenu.y = y;
            state = "shown";
        }
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
            if (textEdit.activeFocus && show)
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
                textEdit.forceActiveFocus();
                textEdit.undo();
            }
        }
        IconButton {
            icon: '../icons/redo-white-128.png'
            enabled: textEdit.canRedo
            visible: textEdit.richEditing && textEdit.hasSelection
            onClicked: {
                textEdit.forceActiveFocus();
                textEdit.redo();
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
                textEdit.forceActiveFocus();
                textEdit.cut();
            }
        }
        IconButton {
            icon: '../icons/copy-white-128.png'
            enabled: textEdit.hasSelection
            visible: textEdit.canCopyPaste && textEdit.hasSelection
            onClicked: {
                textEdit.forceActiveFocus();
                textEdit.copy();
            }
        }
        IconButton {
            icon: '../icons/paste-white-128.png'
            visible: textEdit.canCopyPaste && textEdit.canPaste
            onClicked: {
                textEdit.forceActiveFocus();
                textEdit.paste();
            }
        }
        IconButton {
            icon: '../icons/all-white-128.png'
            enabled: textEdit.length
            visible: textEdit.canCopyPaste && textEdit.selectedText.length < textEdit.length
            onClicked: {
                textEdit.forceActiveFocus();
                textEdit.selectAll();
                textEdit.setHandlePositions();
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
                textEdit.forceActiveFocus();
                handler.italic = !handler.italic;
            }
        }
    }
}
