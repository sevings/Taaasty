import QtQuick 2.0
import QtQuick.Controls 2.0 as Q

Item {
    property alias running: busy.running
    property bool footer: true
    height: visible ? footer ? busy.height + 2 * mm 
                             : busy.height : 0
    width: window.width
//    visible: running
    Q.BusyIndicator {
        id: busy
        height: 7 * mm
        width: height
        anchors {
            top: footer ? parent.top : undefined
            bottom: footer ? undefined : parent.bottom
            horizontalCenter: parent.horizontalCenter
        }
    }
}
