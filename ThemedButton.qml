import QtQuick 2.3

MouseArea {
    id: button
    anchors.margins: 10
    height: 80
    property color textColor: window.textColor
    property string text
    property int fontSize: window.fontNormal
    property bool checked: false
    Rectangle {
        anchors.fill: parent
        radius: 10
        gradient: Gradient {
            GradientStop {
                position: 0
                color: {
                    if (!button.enabled || button.pressed)
                        '#000000';
                    else
                        '#575757';
                }
            }
            GradientStop {
                position: 1
                color: {
                    if (!button.enabled || button.pressed)
                        '#000000';
                    else if (button.checked)
                        window.brightColor;
                    else
                        '#373737';
                }
            }
        }
        Text {
            id: label
            color: button.textColor
            text: button.text
            anchors.centerIn: parent
            anchors.margins: 20
            verticalAlignment: Text.AlignVCenter
            font.pointSize: button.fontSize
            font.family: 'Open Sans'
        }
    }
}
