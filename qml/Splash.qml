import QtQuick 2.7
import QtQuick.Controls 2.0 as Q
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.3

ColumnLayout {
    spacing: 2 * mm
    property bool running: true
    property string text: ''
    anchors.centerIn: parent
    Q.BusyIndicator {
        running: parent.visible && parent.running
        Layout.alignment: Qt.AlignCenter
    }
    Q.Label {
        id: loadingText
        width: window.width
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: window.fontBigger
        wrapMode: Text.Wrap
        text: parent.running ? 'Загрузка…' : parent.text
        color: window.secondaryTextColor
    }
}

