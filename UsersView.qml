import QtQuick 2.5
import org.binque.taaasty 1.0

Rectangle {
    id: back
    color: window.backgroundColor
    signal popped
    property bool poppable
    property string title: ''
    readonly property bool customTitle: bayesMode
    readonly property bool bayesMode: mode === UsersModel.FireMode || mode === UsersModel.WaterMode
    property int mode: UsersModel.MyFollowingsMode
    property int tlogId
    property Tlog tlog: Tlog {
        tlogId: back.tlogId
    }
    signal tlogRequested(int tlog)
    signal profileRequested(int tlog)
    onModeChanged: {
        if (mode === UsersModel.FollowersMode)
            title = 'Подписчики';
        else if (mode === UsersModel.FollowingsMode)
            title = 'Подписки';
        else if (mode === UsersModel.FireMode)
            title = 'Интересные тлоги';
        else if (mode === UsersModel.WaterMode)
            title = 'Неинтересные тлоги';
    }
    Poppable {
        body: back
        Text {
            visible: !users.visible && !usersModel.hasMore
            anchors.centerIn: parent
            color: window.secondaryTextColor
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: window.fontBiggest
            wrapMode: Text.Wrap
            text: 'Список пуст'
        }
    }
    Splash {
        visible: !users.visible && usersModel.hasMore
    }
    MyListView {
        id: users
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        visible: count > 0
        height: contentHeight > parent.height ? parent.height : contentHeight
        model: UsersModel {
            id: usersModel
            mode: back.mode
            tlog: back.tlogId
        }
        delegate: Rectangle {
            width: window.width
            height: usersAvatar.height + 2 * mm
            color: usersMouse.pressed ? window.greenColor : window.backgroundColor
            Poppable {
                id: usersMouse
                anchors.fill: parent
                body: back
                onClicked: {
                    tlogRequested(user.id);
//                if (window.secondMode === 'bayes')
//                    Bayes.toggleInclude(users.type, id);
//                else
//                    Ctrl.showTlogById(id);
                }
            }
            SmallAvatar {
                id: usersAvatar
                anchors.top: undefined
                anchors.verticalCenter: parent.verticalCenter
                anchors.margins: 1 * mm
                user: model.user
                Poppable {
                    anchors.fill: parent
                    body: back
                    onClicked: {
                        profileRequested(user.id);
                    }
                }
            }
            Text {
                id: usersName
                font.pointSize: window.fontBigger
                color: window.textColor //(window.secondMode === 'bayes' && !bayes_include) ? '#808080' : window.textColor
                text: user.name
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: usersAvatar.right
                anchors.leftMargin: 1 * mm
                anchors.rightMargin: 1 * mm
                elide: Text.ElideRight
            }
        }
        header: Item {
            width: window.width
            height: usersTitle.paintedHeight + 2 * mm
            Text {
                id: usersTitle
                font.pointSize: window.fontBigger
                color: window.textColor
                text: back.title
                anchors.centerIn: parent
                y: 1 * mm
            }
        }
        //    footer: Item {
        //        anchors.left: parent.left
        //        anchors.right: parent.right
        //        height: usersAdd.visible ? usersAdd.height : 0
        //        MessageEditor {
        //            id: usersAdd
        //            visible: window.secondMode === 'bayes'
        //            onSent: Bayes.addTlog(users.type, usersAdd.message.trim());
        //        }
        //    }
    }
}
