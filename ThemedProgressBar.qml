import QtQuick 2.7
import QtQuick.Templates 2.0

ProgressBar {
    id: control
    implicitHeight: background.implicitHeight
    implicitWidth: 40 * mm
    padding: 1 * mm
    property string text: ''
    property bool percents: false
//    bottomPadding: background.height - contentItem.height
    contentItem: Item {
        implicitWidth: background.implicitWidth
        implicitHeight: 1 * mm
//        x: control.leftPadding
//        y: control.topPadding + control.availableHeight - height

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
//        x: control.leftPadding
//        y: control.topPadding + (control.availableHeight - height) / 2
//        border.width: 4
//        border.color: 'white'
        ThemedText {
            id: label
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            text: control.text + (showValue <= control.to && control.to > 0
                                  ? ' — ' + (control.percents ? Math.round(showValue / control.to * 100) + '%'
                                                              : showValue + '/' + control.to) : '')
            property int showValue: control.value
            Behavior on showValue {
                NumberAnimation { duration: 200 }
            }
        }
    }
}
