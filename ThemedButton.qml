import QtQuick 2.7

MouseArea {
    id: button
    anchors.margins: 1 * mm
    height: 8 * mm
    property color textColor: window.textColor
    property string text
    property int fontSize: window.fontNormal
    property bool checked: false
    Rectangle {
        anchors.fill: parent
        radius: 1 * mm
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
                        window.greenColor;
                    else
                        '#373737';
                }
            }
        }
        Text {
            id: label
            color: button.textColor
            text: button.text
            anchors.fill: parent
            anchors.margins: 2 * mm
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
            font.pointSize: button.fontSize
        }
    }
}
