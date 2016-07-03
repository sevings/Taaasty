import QtQuick 2.7
import QtQuick.Templates 2.0

TextField {
    id: back
    anchors.margins: 1 * mm
    padding: 1 * mm
    color: window.textColor
    font.pointSize: window.fontNormal
    implicitHeight: contentHeight + 2 * mm
    implicitWidth: 30 * mm
    background: Rectangle {
        gradient: Gradient {
            GradientStop { position: 0; color: window.darkTheme ? 'black'   : '#adbac6' }
            GradientStop { position: 1; color: window.darkTheme ? '#181818' : 'white' }
        }
        border.color: window.secondaryTextColor
        border.width: 0.1 * mm
        radius: 0.8 * mm
    }
}