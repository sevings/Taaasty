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

Pane {
    id: dialog
    innerFlick: flick
    readonly property bool isLoginDialog: true // how to find this in the StackView?
    function clear() {
        mail.line = '';
        password.line = '';
    }
    Poppable {
        body: dialog
    }
    MyFlickable {
        id: flick
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            right: parent.right
        }
        height: contentHeight > parent.height
                ? parent.height : contentHeight
        topMargin: 2 * mm
        bottomMargin: 2 * mm
        contentWidth: parent.width
        contentHeight: column.height
        Poppable {
            body: dialog
        }
        Column {
            id: column
            width: parent.width
            spacing: 2 * mm
            Q.Label {
                id: mailLabel
                text: 'E-mail или ник'
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: window.fontBigger
            }
            LineInput {
                id: mail
                anchors.horizontalCenter: parent.horizontalCenter
                width: 40 * mm
                onAccepted: {
                    if (loginButton.enabled)
                        Tasty.authorize(mail.text, password.text, saveBox.checked);
                    else
                        password.focus = true;
                }
            }
            Q.Label {
                id: passwordLabel
                text: 'Пароль'
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: window.fontBigger
            }
            LineInput {
                id: password
                echoMode: TextInput.Password
                anchors.horizontalCenter: parent.horizontalCenter
                width: 40 * mm
                onAccepted: {
                    if (loginButton.enabled)
                        Tasty.authorize(mail.text, password.text, saveBox.checked);
                }
            }
            ThemedCheckBox {
                id: saveBox
                anchors.horizontalCenter: parent.horizontalCenter
                width: 40 * mm
                text: 'Сохранить'
                checked: true
            }
            ThemedButton {
                id: loginButton
                anchors.horizontalCenter: parent.horizontalCenter
                text: 'Войти'
                width: 40 * mm
                onClicked: Tasty.authorize(mail.text, password.text, saveBox.checked)
                enabled: mail.text && password.text
            }
            ThemedButton {
                id: resetButton
                anchors.horizontalCenter: parent.horizontalCenter
                text: 'Сбросить пароль'
                width: 40 * mm
                enabled: mail.text
                onClicked: Tasty.resetPassword(mail.text)
                Connections {
                    target: Tasty
                    onResetMailSent: resetDialog.open()
                }
            }
            HelpPageItem {
                anchors.horizontalCenter: parent.horizontalCenter
                width: 40 * mm
                popBody: dialog
                text: 'ВКонтакте или Facebook'
                entryId: 21623665
            }
        //    ThemedButton {
        //        id: registerButton
        //        text: 'Зарегистрироваться'
        //        anchors.top: loginButton.bottom
        //        anchors.left: parent.left
        //        anchors.right: parent.right
        //        anchors.margins: 2 * mm
        //        enabled: mail.text && password.text
                //onClicked: Ctrl.signup()
        //    }
        }
    }
    PopupDialog {
        id: resetDialog
        isQuestion: false
        text: 'На указанную при регистрации почту отправлено '
              + 'письмо с информацией для восстановления пароля.'
    }
}
