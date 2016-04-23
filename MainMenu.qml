import QtQuick 2.3
import org.binque.taaasty 1.0

MouseArea {
    id: area
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    property int minWidth: parent.width * 0.7
    width: x > drag.minimumX ? parent.width : minWidth + 20
    x: - minWidth
    drag.target: area
    drag.axis: Drag.XAxis
    drag.minimumX: - minWidth
    drag.maximumX: 0
    propagateComposedEvents: true
    enabled: false
    signal modeChanged(int mode)
    onModeChanged: backAnimation.start();
    Component.onCompleted: {
        enabled = true;
    }
    onReleased: {
        if (area.x > -area.minWidth / 3 * 2)
            forwardAnimation.start();
        else
            backAnimation.start();
    }
    onClicked: {
        if (mouse.x > minWidth)
            backAnimation.start();
    }
    PropertyAnimation {
        id: backAnimation
        target: area
        property: "x"
        to: - minWidth
        duration: 100
    }
    PropertyAnimation {
        id: forwardAnimation
        target: area
        property: "x"
        to: 0
        duration: 100
    }
    Rectangle {
        id: menu
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        width: parent.minWidth
        color: window.backgroundColor
        Column {
            anchors.fill: parent
            spacing: 10
            ThemedButton {
                anchors.left: parent.left
                anchors.right: parent.right
                text: 'My tlog'
                onClicked: { modeChanged(FeedModel.MyTlogMode) }
            }
            ThemedButton {
                anchors.left: parent.left
                anchors.right: parent.right
                text: 'Friends'
                onClicked: { modeChanged(FeedModel.FriendsMode) }
            }
            ThemedButton {
                anchors.left: parent.left
                anchors.right: parent.right
                text: 'Live'
                onClicked: { modeChanged(FeedModel.LiveMode) }
            }
            ThemedButton {
                anchors.left: parent.left
                anchors.right: parent.right
                text: 'Anonymous'
                onClicked: { modeChanged(FeedModel.AnonymousMode) }
            }
            ThemedButton {
                anchors.left: parent.left
                anchors.right: parent.right
                text: 'Best'
                onClicked: { modeChanged(FeedModel.BestMode) }
            }
        }
    }
}
