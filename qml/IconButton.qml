/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Quick Controls 2 module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.9
import QtQuick.Templates 2.1 as T
import QtQuick.Controls.Material 2.2
import QtQuick.Controls.Material.impl 2.1

T.ToolButton {
    id: control
    anchors.margins: 1.5 * mm
    property url icon: ''
    property int iconHeight: 20 * sp
    font.pixelSize: window.fontNormal
    implicitWidth: Math.max(background ? background.implicitWidth : 0,
                            contentItem.implicitWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(background ? background.implicitHeight : 0,
                             contentItem.implicitHeight + topPadding + bottomPadding)
    baselineOffset: contentItem.y + contentItem.baselineOffset

    padding: 6

    contentItem: Item {
        implicitHeight: Math.max(control.iconHeight, contentText.height)
        implicitWidth: row.width + 3 * mm
        Row {
            id: row
            anchors.centerIn: parent
            spacing: 1.5 * mm
            Image {
                source: control.icon
                height: control.iconHeight
                fillMode: Image.PreserveAspectFit
            }
            Text {
                id: contentText
                text: control.text
                font: control.font
                color: !control.enabled ? control.Material.hintTextColor :
                                          control.checked || control.highlighted ? control.Material.accent : control.Material.foreground
                visible: control.text.length
                verticalAlignment: Text.AlignVCenter
                height: parent.height
            }
        }
    }

    background: Rectangle {
        implicitWidth: 48 * sp
        implicitHeight: 48 * sp
        color: control.down || control.highlighted ? control.Material.rippleColor : control.Material.buttonColor
        visible: control.enabled && (control.down || control.visualFocus || control.checked || control.highlighted)
    }
}
