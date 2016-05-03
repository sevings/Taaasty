import QtQuick 2.3

Rectangle {
    id: bar
    width: parent.width / 5
    height: 0.5 * mm
    color: window.textColor
    x: 0
//    y: busy ? 0 : -height
    visible: busy
    property bool busy: false
    SequentialAnimation {
        running: bar.busy
        loops: Animation.Infinite
        XAnimator {
            target: bar
            from: 0
            to: window.width * 0.8
            duration: 500
        }
        XAnimator {
            target: bar
            from: window.width * 0.8
            to: 0
            duration: 500
        }
    }
}
