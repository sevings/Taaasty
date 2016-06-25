import QtQuick 2.7
import QtQuick.Templates 2.0

Button {
    id: button
    anchors.margins: 1 * mm
    implicitHeight: 8 * mm
    implicitWidth: 20 * mm
    property bool glowing: false
    property color glowColor: window.greenColor
    font.pointSize: window.fontNormal
    padding: 2 * mm
    contentItem: Text {
        text: button.text
        font: button.font
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.Wrap
        color: enabled ? window.textColor : window.secondaryTextColor
    }
    background: Rectangle {
        radius: 1 * mm
        visible: button.enabled && !button.pressed
        gradient: Gradient {
            GradientStop {
                position: 0
                color: {
                        window.darkTheme ? '#575757' : '#eff3f7';
                }
            }
            GradientStop {
                position: 1
                color: {
                    if (button.glowing)
                        button.glowColor;
                    else
                        window.darkTheme ? '#373737' : '#b5babd';
                }
            }
        }
    }
}
