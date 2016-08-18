import QtQuick 2.7
import QtQuick.Controls 2.0 as Q
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.3

ColumnLayout {
    id: control
    spacing: 1 * mm
    property string text: ''
    property bool percents: false
    property alias to: bar.to
    property int value: 0
    property int showValue: value
    Behavior on showValue {
        NumberAnimation { duration: 200 }
    }
    Q.ProgressBar {
        id: bar
        Layout.preferredWidth: window.width
        padding: 1 * mm
        value: control.showValue
    }
    ThemedText {
        id: label
        leftPadding: 5 * mm
        rightPadding: leftPadding
        Layout.preferredWidth: window.width
        text: control.text + (control.showValue <= control.to && control.to > 0
                              ? ' — ' + (control.percents ? Math.round(control.showValue / control.to * 100) + '%'
                                                          : control.showValue + '/' + control.to) : '')
    }
}
