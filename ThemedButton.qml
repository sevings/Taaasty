import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Button {
    id: button
//    anchors.left: parent.left
//    anchors.right: parent.right
    anchors.margins: 10
    height: 80
    property int fontSize: 25
    style: ButtonStyle {
        background: Rectangle {
            implicitHeight: 60
            implicitWidth: control.width
            radius: 10
            //color: button.enabled ? '#333' : backgroundColor
            gradient: Gradient {
                GradientStop { position: 0; color: control.enabled ? '#575757' : '#000000' }
                GradientStop { position: 1; color: control.enabled ? control.pressed ? window.brightColor : '#373737' : '#000000' }
            }
        }
        label: Text {
            id: label
            color: window.textColor
            text: control.text
            anchors.margins: 20
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: control.fontSize
            font.family: 'Open Sans'
        }
    }
}


