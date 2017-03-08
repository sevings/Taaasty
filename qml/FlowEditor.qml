/*
 * Copyright (C) 2016 Vasily Khodakov
 * Contact: <binque@ya.ru>
 *
 * This file is part of Taaasty.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.8
import QtQuick.Controls 2.1 as Q
import QtQuick.Controls.Material 2.1
import org.binque.taaasty 1.0

Pane {
    id: back
    property Flow editFlow
    readonly property bool isFlowEditor: true
    onHeightChanged: {
        if (titleInput.activeFocus)
            titleInput.ensureVisible(titleInput.cursorRectangle);
    }
    Component.onCompleted: {
        if (!editFlow)
            return;

        editor.setFlow(editFlow);

        nameText.text     = editFlow.name;
        slugText.text     = 'http://taaasty.com/~' + editFlow.slug;
        titleInput.text   = editFlow.title;
        lockButton.locked = editFlow.isPrivate;
    }
    Poppable {
        body: back
    }
    MyFlickable {
        id: flick
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        height: Math.min(back.height, contentHeight + 1.5 * mm)
        bottomMargin: 1.5 * mm
        contentWidth: parent.width
        contentHeight: column.height
        onContentHeightChanged: returnToBounds()
        Poppable {
            body: back
        }
        Column {
            id: column
            width: window.width
            spacing: 1.5 * mm
            AnimatedImage {
                id: picture
                width: parent.width
                height: width * 9 / 16
                cache: true
                asynchronous: false
                autoTransform: true
                fillMode: Image.PreserveAspectCrop
                source: editor.pic
                Rectangle {
                    visible: removeImageButton.visible
                    anchors.fill: removeImageButton
                    color: window.backgroundColor
                    opacity: 0.3
                    radius: width / 2
                }
                IconButton {
                    id: removeImageButton
                    enabled: !editor.loading
                    visible: editor.pic
                    anchors {
                        top: parent.top
                        right: parent.right
                    }
                    icon: (window.darkTheme ? '../icons/cross-white'
                                            : '../icons/cross-black')
                          + '-128.png'
                    onClicked: editor.clearPic()
                }
                MouseArea {
                    anchors.fill: parent
                    visible: !editor.loading && !editor.pic
                    enabled: visible
                    scale: pressed ? 0.9 : 1
                    onClicked: editor.changePic()
                    Behavior on scale {
                        NumberAnimation { easing.overshoot: 5; easing.type: Easing.OutBack; duration: 400; }
                    }
                    Rectangle {
                        anchors.fill: parent
                        color: window.darkTheme ? Qt.darker('#9E9E9E') : '#9E9E9E'
                    }
                    ThemedText {
                        anchors.centerIn: parent
                        text: 'Выбрать фон'
                        font.pixelSize: window.fontBigger
                    }
                }
            }
            ThemedCheckBox {
                id: optimizeBox
                enabled: !editor.loading
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: 1.5 * mm
                }
                text: 'Оптимизировать размер'
                onCheckedChanged: { Settings.lastOptimizeImages = checked; }
                checked: Settings.lastOptimizeImages
            }
            Q.TextField {
                id: nameText
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: 1.5 * mm
                    topMargin: 0
                }
                placeholderText: 'Название потока'
                font.pixelSize: window.fontNormal
            }
            Q.TextField {
                id: slugText
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: 1.5 * mm
                    topMargin: 0
                }
                visible: editFlow
                validator: RegExpValidator {
                    regExp: /http:\/\/taaasty\.com\/~[a-zA-Z0-9_\.]+/
                }
                font.pixelSize: window.fontNormal
                onTextChanged: {
                    if (length < 20)
                        text = 'http://taaasty.com/~';
                }
            }
            TextEditor {
                id: titleInput
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 3 * mm
                z: 6
                readOnly: editor.loading
                flickable: flick
                placeholderText: 'Краткое описание'
                popBody: back
                onActiveFocusChanged: {
                    if (!activeFocus)
                        return;

                    window.hideFooter();
                }
            }
            Row {
                id: buttonsRow
                spacing: 1.5 * mm
                anchors {
                    right: parent.right
                    leftMargin: 1.5 * mm
                    rightMargin: 1.5 * mm
                }
                IconButton {
                    id: lockButton
                    visible: editFlow
                    property bool locked
                    icon: (locked ? (window.darkTheme ? '../icons/lock-white-'
                                                      : '../icons/lock-black-')
                                  : (window.darkTheme ? '../icons/unlock-white-'
                                                      : '../icons/unlock-black-'))
                          + '128.png'
                    onClicked: {
                        locked = !locked;
                    }
                }
                IconButton {
                    id: postButton
                    enabled: nameText.length && (!editFlow || slugText.acceptableInput)
                    icon: (window.darkTheme ? '../icons/send-light-'
                                            : '../icons/send-dark-')
                          + '128.png'
                    onClicked: {
                        var slug = slugText.text.replace('http://taaasty.com/~', '')
                        if (editFlow)
                            editor.update(editFlow.id, nameText.text, titleInput.text,
                                          slug, lockButton.locked, false);
                        else
                            editor.create(nameText.text, titleInput.text);
                    }
                }
            }
            ThemedProgressBar {
                id: uploadBar
                visible: editor.loading
                text: 'Отправка'
                units: 'КБ'
                value: editor.kBytesSent
                to: editor.kBytesTotal
            }
        }
    }
    MyScrollIndicator { flick: flick }
    FlowEditor {
        id: editor
        onCreated: {
            window.showMessage('Поток создан', true);
            window.popFromStack();
        }
        onEdited: {
            window.showMessage('Поток отредактирован', true);
            window.popFromStack();
        }
    }
}
