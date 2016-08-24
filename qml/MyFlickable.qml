import QtQuick 2.7
import QtQuick.Controls.Material 2.0
import org.binque.taaasty 1.0

Flickable {
    id: list
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
