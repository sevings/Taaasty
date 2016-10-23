/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.7
import QtQuick.Controls 2.0 as Q
import QtQuick.Controls.Material 2.0
import QtQuick.Controls.Material.impl 2.0

Rectangle {
    id: page
    visible: opacity > 0
    property bool info: false
    property bool permanent: false
    signal opened
    signal closed
    width: dialogText.contentWidth + 3 * mm
    height: dialogText.contentHeight + 3 * mm
    anchors {
        horizontalCenter: parent.horizontalCenter
        bottomMargin: 60 * sp
        bottom: parent.bottom
    }
    color: window.backgroundColor
    border.width: 0.2 * mm
    border.color: info ? window.greenColor : window.redColor
    opacity: 0
    layer.enabled: visible
    layer.effect: ElevationEffect {
        elevation: 24
    }
    Behavior on opacity {
        NumberAnimation { duration: 300 }
    }
    function close() {
        if(page.opacity == 0)
            return;

        closed();
        page.opacity = 0;
    }
    function show(txt, info, permanent) {
        opened();
        dialogText.text = txt;
        page.opacity = 1;
        page.info = info === true;
        page.permanent = permanent === true;
        if (permanent !== true)
            timer.start()
        else
            timer.stop();
    }
    Timer {
        id: timer
        interval: 5000
        repeat: false
        onTriggered: page.close()
    }
    Q.Label {
        id: dialogText
        anchors.centerIn: parent
        width: window.width - 6 * mm
        text: "Hello World!"
        font.pointSize: window.fontSmaller
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
    }
}
