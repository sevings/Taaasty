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
    property bool pauseAnimations: false
    layer.enabled: true// x > 0 && opacity === 1
    layer.effect: ElevationEffect {
        elevation: 24
    }
    property Flickable innerFlick
    Connections {
        target: innerFlick
        onMovementStarted: {
            pauseAnimations = pane.x > 0 || innerFlick.movingVertically
        }
        onMovementEnded: {
            pauseAnimations = pane.x > 0 || innerFlick.movingVertically
        }
    }
    onXChanged: {
        pauseAnimations = pane.x > 0 || (innerFlick && innerFlick.movingVertically)
    }
}
