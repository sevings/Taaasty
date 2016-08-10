import QtQuick 2.0
import QtQuick.Controls 2.0 as Q

Item {
    property alias running: busy.running
    property bool footer: true
    height: visible ? busy.height + 2 * mm : 0
    width: window.width
    visible: running
    Q.BusyIndicator {
        id: busy
        height: 5 * mm
        width: 5 * mm
        anchors {
            top: footer ? parent.top : undefined
            bottom: footer ? undefined : parent.bottom
            horizontalCenter: parent.horizontalCenter
        }
    }
}
