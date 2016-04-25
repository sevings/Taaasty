import QtQuick 2.3
import org.binque.taaasty 1.0

Rectangle {
    id: back
    color: window.backgroundColor
    signal popped
    property bool poppable
    property string title: 'Мои подписки'
    property int mode: UsersModel.MyFollowingsMode
    property string tlog
    signal tlogRequested(int tlog)
    onModeChanged: {
        if (mode === UsersModel.FollowersMode)
            title = 'Подписчики';
        else if (mode === UsersModel.FollowingsMode)
            title = 'Подписки';
        //        else if (type === 'fire')
        //            title = 'Интересные тлоги';
        //        else if (type === 'water')
        //            title = 'Неинтересные тлоги';
    }
    Poppable {
        body: back
        Text {
            visible: users.count === 0 && !usersModel.hasMore
            anchors.centerIn: parent
            color: window.secondaryTextColor
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: 30
            wrapMode: Text.Wrap
            text: 'Список пуст'
        }
    }
    MyListView {
        id: users
        anchors.fill: parent
        model: UsersModel {
            id: usersModel
            mode: back.mode
            tlog: back.tlog
        }
        delegate: Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            height: usersAvatar.height + 20
            color: usersMouse.pressed ? window.brightColor : window.backgroundColor
            SmallAvatar {
                id: usersAvatar
                anchors.top: undefined
                anchors.verticalCenter: parent.verticalCenter
                anchors.margins: 10
                source: user.thumb64
                symbol: user.symbol
            }
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
            Text {
                id: usersName
                font.pointSize: 25
                color: window.textColor //(window.secondMode === 'bayes' && !bayes_include) ? '#808080' : window.textColor
                text: user.name
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: usersAvatar.right
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                elide: Text.ElideRight
            }
        }
        header: Item {
            anchors.left: parent.left
            anchors.right: parent.right
            height: usersTitle.paintedHeight + 20
            Text {
                id: usersTitle
                font.pointSize: 25
                color: window.textColor
                text: back.title
                anchors.centerIn: parent
                y: 10
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
