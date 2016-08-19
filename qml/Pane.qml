import QtQuick 2.7
import QtQuick.Controls 2.0 as Q
import QtQuick.Controls.Material 2.0
import QtQuick.Controls.Material.impl 2.0

Q.Pane {
    id: pane
    padding: 0
    property bool poppable
    signal popped
    onPopped: window.popFromStack()
    property Flickable innerFlick
    property bool pauseAnimations: pane.x > 0 || (innerFlick && innerFlick.movingVertically)
//    layer.enabled: !innerFlick || !innerFlick.movingVertically
//    layer.sourceRect: Qt.rect(0, 0, 100, window.height)
//    layer.effect: ElevationEffect {
//        elevation: 24
//    }
}
