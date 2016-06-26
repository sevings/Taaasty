import QtQuick 2.0
import org.binque.taaasty 1.0

Rectangle {
    id: back
    color: window.backgroundColor
    onPopped: window.popFromStack()
    signal popped
    property bool poppable
    property string where: ''
    Component.onCompleted: {
        titleInput.text = Settings.lastTitle;
        textInput.text  = Settings.lastText;
    }
    Poppable {
        body: back
    }
    LineInput {
        id: titleInput
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
    }
    TextEditor {
        id: textInput
        anchors.top: titleInput.bottom
        anchors.bottom: formatButtons.top
        anchors.left: parent.left
        anchors.right: parent.right
    }
    Row {
        id: formatButtons
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: post.top
        anchors.margins: 1 * mm
        spacing: 1 * mm
        ThemedButton {
            id: italic
            width: (parent.width - 5 * 3) / 4
            height: 6 * mm
            text: '<i>I</i>'
            onClicked: textInput.insertTags('<i>', '</i>')
        }
        ThemedButton {
            width: italic.width
            height: italic.height
            text: '<b>B</b>'
            onClicked: textInput.insertTags('<b>', '</b>')
        }
        ThemedButton {
            width: italic.width
            height: italic.height
            text: '<u>U</u>'
            onClicked: textInput.insertTags('<u>', '</u>')
        }
        ThemedButton {
            width: italic.width
            height: italic.height
            text: 'http'
            onClicked: textInput.insertTags('<a href=\"', '\"></a>')
        }
    }
    ThemedButton {
        id: post
        anchors.bottom: notNow.top
        anchors.left: parent.left
        anchors.right: parent.right
        text: 'Отправить ' + back.where
        onClicked: {
//            Settings.lastTitle = '';
//            Settings.lastText  = '';
        }
    }
    ThemedButton {
        id: notNow
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        text: 'Позже'
        onClicked: {
            Settings.lastTitle = titleInput.text;
            Settings.lastText  = textInput.text;
            back.popped();
        }
    }
}
