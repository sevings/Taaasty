import QtQuick 2.7
import QtQuick.Controls.Material 2.0
import org.binque.taaasty 1.0

Pane {
    id: back
    innerFlick: listView
    property int mode: Tasty.isAuthorized ? FlowsModel.MyFlowsMode : FlowsModel.AllFlowsMode
    readonly property string title: { // for footer
        switch (back.mode) {
        case FlowsModel.MyFlowsMode:
            'Мои потоки'; break;
        case FlowsModel.AllFlowsMode:
            'Все потоки'; break;
        default:
            'Тейсти'
        }
    }
    readonly property bool customTitle: true
    readonly property bool isFlowsView: true
    Poppable {
        body: back
        Text {
            visible: !listView.visible && !flowsModel.hasMore
            anchors.centerIn: parent
            color: window.secondaryTextColor
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: window.fontBigger
            wrapMode: Text.Wrap
            text: 'Нет потоков'
        }
    }
    Splash {
        visible: !listView.visible && flowsModel.hasMore && !pauseAnimations
        running: !flowsModel.isPrivate
    }
    MyListView {
        id: listView
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: contentHeight > parent.height ? parent.height : contentHeight
        visible: count > 0
        model: FlowsModel {
            id: flowsModel
            mode: back.mode
        }
        delegate: Item {
            width: window.width
            height: 7 * mm + flowPosts.y + flowPosts.height
            Poppable {
                body: back
                onClicked: {
                    if (back.x > 0) {
                        mouse.accepted = false;
                        return;
                    }

                    mouse.accepted = true;
                    window.pushTlog(flow.id);
                }
            }
            MyImage {
                id: flowPicture
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottomMargin: 1 * mm
                url: flow.picUrl
                width: window.width
                height: width * 10 / 16
                fillMode: Image.PreserveAspectCrop
                paused: pauseAnimations
            }
            ThemedText {
                id: flowName
                text: flow.name
                anchors.top: flowPicture.bottom
                anchors.left: parent.left
                anchors.right: parent.right
            }
            ThemedText {
                id: flowTitle
                text: flow.title
                font.pointSize: window.fontSmaller
                anchors.top: flowName.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                height: text.length === 0 ? -1 * mm : paintedHeight
            }
            ThemedText {
                id: flowFollowers
                text: flow.followersCount
                color: window.secondaryTextColor
                font.pointSize: window.fontSmallest
                anchors.top: flowTitle.bottom
                anchors.right: parent.right
            }
            ThemedText {
                id: flowPosts
                text: flow.entriesCount
                color: window.secondaryTextColor
                font.pointSize: window.fontSmallest
                anchors.top: flowTitle.bottom
                anchors.left: parent.left
            }
        }
        footer: ListBusyIndicator {
            running: flowsModel.loading && !pauseAnimations
        }
    }
}
