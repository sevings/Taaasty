import QtQuick 2.3
import org.binque.taaasty 1.0

MouseArea {
    id: area
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    property int minWidth: parent.width * 0.7
    width: x > drag.minimumX ? parent.width : minWidth + 5 * mm
    x: - minWidth
    drag.target: area
    drag.axis: Drag.XAxis
    drag.minimumX: - minWidth
    drag.maximumX: 0
    propagateComposedEvents: true
    enabled: false
    signal modeChanged(int mode)
    signal settingsRequested
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
        Flickable {
            anchors.fill: parent
            flickableDirection: Flickable.VerticalFlick
            topMargin: 1 * mm
            bottomMargin: 1 * mm
            contentWidth: parent.width
            contentHeight: 9 * mm * 11 - mm
            Column {
                id: column
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
                ThemedButton {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    text: 'Excellent'
                    onClicked: { modeChanged(FeedModel.ExcellentMode) }
                }
                ThemedButton {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    text: 'Well'
                    onClicked: { modeChanged(FeedModel.WellMode) }
                }
                ThemedButton {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    text: 'Good'
                    onClicked: { modeChanged(FeedModel.GoodMode) }
                }
                ThemedButton {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    text: 'Favorites'
                    onClicked: { modeChanged(FeedModel.FavoritesMode) }
                }
                ThemedButton {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    text: 'Tlog'
                    onClicked: { modeChanged(FeedModel.TlogMode) }
                }
                ThemedButton {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    text: 'Settings'
                    onClicked: { area.settingsRequested() }
                }
            }
        }
    }
}
