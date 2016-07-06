import QtQuick 2.7
import QtQuick.Templates 2.0

ProgressBar {
    id: control
    implicitHeight: background.implicitHeight
    implicitWidth: 40 * mm
    padding: 1 * mm
    property string text: ''
    property bool percents: false
    contentItem: Item {
        implicitWidth: background.implicitWidth
        implicitHeight: 1 * mm

        Rectangle {
            anchors.bottom: parent.bottom
            width: control.visualPosition * control.availableWidth
            height: parent.implicitHeight
            radius: 0.3 * mm
            color: window.greenColor
        }
    }
    background: Rectangle {
        color: window.backgroundColor
        implicitWidth: control.implicitWidth
        implicitHeight: 1 * mm + label.height + 1 * mm + control.padding * 2
        ThemedText {
            id: label
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                leftMargin: 5 * mm
                rightMargin: anchors.leftMargin
            }
            text: control.text + (control.value <= control.to && control.to > 0
                                  ? ' — ' + (control.percents ? Math.round(control.value / control.to * 100) + '%'
                                                              : control.value + '/' + control.to) : '')
        }
    }
}
