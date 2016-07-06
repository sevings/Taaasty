import QtQuick 2.7
import org.binque.taaasty 1.0

ListView {
    id: list
    highlightFollowsCurrentItem: false
    cacheBuffer: window.height * 2
    flickableDirection: Flickable.VerticalFlick
    maximumFlickVelocity: 1000 * mm
    interactive: parent.x <= 0
    property bool poppable: true
    boundsBehavior: Flickable.DragOverBounds
    onVerticalVelocityChanged: {
        if (!dragging)
            return;

        if (verticalVelocity > 0) {
            window.hideFooter();
//            upButton.state = "down";
        }
        else if (verticalVelocity < 0) {
            window.showFooter();
//            upButton.state = "up";
        }
    }
    add: Transition {
        NumberAnimation { property: "opacity"; from: 0;   to: 1.0; duration: 300 }
        NumberAnimation { property: "scale";   from: 0.8; to: 1.0; duration: 300 }
    }
    remove: Transition {
        NumberAnimation { property: "opacity"; from: 1.0; to: 0; duration: 300 }
        NumberAnimation { property: "scale";   from: 1.0; to: 0.8; duration: 300 }
    }
    displaced: Transition {
        NumberAnimation { property: "y"; duration: 300 }
    }
    Rectangle {
        id: scrollbar
        anchors {
            right: parent.right
            margins: 0.5 * mm
        }
        y: parent.visibleArea.yPosition * (parent.height - height + h)
        width: 1 * mm
        property int h: parent.visibleArea.heightRatio * parent.height
        height: h > 5 * mm ? h : 5 * mm
        color: window.textColor
        opacity: parent.movingVertically ? 0.7 : 0
        visible: opacity > 0 && height < parent.height * 0.9
        Behavior on opacity {
            NumberAnimation { duration: 500 }
        }
        Behavior on height {
            NumberAnimation { duration: 500 }
        }
        radius: 0.5 * mm
    }
/*    Rectangle {
        id: upButton
        anchors {
            right: parent.right
            margins: 1 * mm
        }
        width: 10 * mm
        height: width
        radius: width / 2
        color: window.textColor
        opacity: 0.3
        visible: y < window.height
        Text {
            anchors.centerIn: parent
            font.pointSize: window.fontBiggest
            text: '^'
            color: window.textColor
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                upButton.state = "down";
                list.positionViewAtBeginning();
            }
        }
        state: "down"
        states: [
            State {
                name: "up"
                PropertyChanges {
                    target: upButton
                    y: window.height - height - 10 * mm
                }
            },
            State {
                name: "down"
                PropertyChanges {
                    target: upButton
                    y: window.height
                }
            }
        ]
        transitions: [
            Transition {
                NumberAnimation {
                    easing.type: Easing.OutCubic
                    duration: 400
                    property: 'y'
                }
            }
        ]
    }*/
}
