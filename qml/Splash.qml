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

import QtQuick 2.7
import QtQuick.Controls 2.0 as Q
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.3
import org.binque.taaasty 1.0

Item {
    id: splash
    property QtObject model
    property bool running: !model || model.loading
    property string text: model ? model.errorString : ''
    property string emptyString: ''
    anchors.centerIn: parent
    width: window.width - 3 * mm
    height: busy.height + loadingText.height + 1.5 * mm
    Q.BusyIndicator {
        id: busy
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
        }
        running: splash.visible && splash.running
    }
    Q.Label {
        id: loadingText
        anchors {
            top: busy.bottom
            horizontalCenter: parent.horizontalCenter
            margins: 1.5 * mm
        }
        width: window.width - 3 * mm
        font.pointSize: window.fontBigger
        wrapMode: Text.Wrap
        horizontalAlignment: Text.AlignHCenter
        text: splash.running ? 'Загрузка…' : (splash.text || splash.emptyString)
    }
    ThemedButton {
        id: reloadButton
        anchors {
            top: loadingText.bottom
            horizontalCenter: parent.horizontalCenter
        }
        implicitWidth: 40 * mm
        highlighted: true
        text: 'Обновить'
        visible: model && model.networkError
        onClicked: {
            if (!model)
                return;

            if (model.id)
                model.reload();
            else
                model.loadMore();
        }
    }
}
