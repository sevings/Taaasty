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
    property alias value: bar.value
    Q.ProgressBar {
        id: bar
        Layout.preferredWidth: window.width
        padding: 1 * mm
    }
    ThemedText {
        id: label
        leftPadding: 5 * mm
        rightPadding: leftPadding
        Layout.preferredWidth: window.width
        text: control.text + (control.value <= control.to && control.to > 0
                              ? ' — ' + (control.percents ? Math.round(control.value / control.to * 100) + '%'
                                                          : control.value + '/' + control.to) : '')
    }
}
