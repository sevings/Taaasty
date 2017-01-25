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
import QtQuick.Controls.Material 2.1
import org.binque.taaasty 1.0

Flickable {
    id: flickable
    flickableDirection: Flickable.VerticalFlick
    maximumFlickVelocity: 1000 * mm
    interactive: parent.x <= 0
    boundsBehavior: Flickable.DragOverBounds
    onVerticalVelocityChanged: {
        if (!dragging)
            return;

        if (verticalVelocity > 0) {
            window.hideFooter();
        }
        else if (verticalVelocity < 0) {
            window.showFooter();
        }
    }
}
