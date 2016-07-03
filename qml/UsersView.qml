import QtQuick 2.7
import org.binque.taaasty 1.0

Rectangle {
    id: back
    color: window.backgroundColor
    signal popped
    onPopped: window.popFromStack()
    property bool poppable
    property string title: ''
    readonly property bool customTitle: bayesMode
    readonly property bool bayesMode: mode === UsersModel.FireMode || mode === UsersModel.WaterMode
    property int mode: UsersModel.MyFollowingsMode
    property int tlogId
    property Tlog tlog: Tlog {
        tlogId: back.tlogId
    }
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
    }
    Splash {
        visible: !users.visible
        text: users.model.hasMore ? 'Загрузка…' : 'Список пуст'
    }
    MyListView {
        id: users
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        visible: count > 0
        height: contentHeight > parent.height ? parent.height : contentHeight
        Component.onCompleted: {
            var qml = 'import org.binque.taaasty 1.0\n' + (bayesMode ? 'UsersModelBayes { } ' : 'UsersModelTlog { } ');
            var mdl = Qt.createQmlObject(qml, users);
            back.modeChanged.connect(function() { mdl.mode = back.mode; } )
            mdl.mode = back.mode;
            if (!bayesMode)
            {
                mdl.tlog = back.tlogId
                back.tlogIdChanged.connect(function() { mdl.tlog = back.tlogId; })
            }

            users.model = mdl;
        }

        delegate: Item {
            width: window.width
            height: usersAvatar.height + 2 * mm
            Rectangle {
                id: fillRect
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                width: parent.width
                color: window.greenColor
                visible: usersMouse.pressed || removing.running
            }
            Poppable {
                id: usersMouse
                anchors.fill: parent
                body: back
                onClicked: {
                    window.pushTlog(user.id);
                    removing.stop();
                }
            }
            SmallAvatar {
                id: usersAvatar
                anchors.top: undefined
                anchors.verticalCenter: parent.verticalCenter
                anchors.margins: 1 * mm
                user: model.user
                Poppable {
                    body: back
                    onClicked: {
                        if (bayesMode)
                        {
                            if (removing.running)
                                removing.stop();
                            else
                                removing.start();
                        }
                        else
                            window.pushProfileById(user.id);
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
                anchors.right: parent.right
                anchors.leftMargin: 1 * mm
                anchors.rightMargin: 1 * mm
                elide: Text.ElideRight
            }
            NumberAnimation {
                id: removing
                target: fillRect
                property: "width"
                easing.type: Easing.OutQuad
                from: window.width
                to: 0
                duration: 4000
                onStopped: {
                    if (fillRect.width > 0)
                        fillRect.width = window.width;
                    else
                        users.model.removeUser(user.id);
                }
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