import QtQuick 2.7
import QtQuick.Controls 2.0 as Q
import QtQuick.Controls.Material 2.0
import org.binque.taaasty 1.0

Pane {
    id: back
    pauseAnimations: x > 0 || users.movingVertically
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
        else if (mode === UsersModel.MyIgnoredMode)
            title = 'Заблокированы';
        else if (mode === UsersModel.FireMode)
            title = 'Интересные тлоги';
        else if (mode === UsersModel.WaterMode)
            title = 'Неинтересные тлоги';
    }
    Poppable {
        body: back
    }
    Splash {
        visible: !users.visible && !pauseAnimations
        running: users.model.hasMore
        text: 'Список пуст'
    }
    MyListView {
        id: users
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
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
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
                }
                width: parent.width
                color: Material.primary
                visible: usersMouse.pressed || removing.running
            }
            Poppable {
                id: usersMouse
                anchors.fill: parent
                body: back
                onClicked: {
                    mouse.accepted = true;
                    window.pushTlog(user.id);
                    removing.stop();
                }
            }
            SmallAvatar {
                id: usersAvatar
                anchors {
                    top: undefined
                    verticalCenter: parent.verticalCenter
                    margins: 1 * mm
                }
                user: model.user
                paused: pauseAnimations
                popBody: back
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
            Q.Label {
                id: usersName
                font.pointSize: window.fontBigger
                text: user.name
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: usersAvatar.right
                    right: parent.right
                    leftMargin: 1 * mm
                    rightMargin: 1 * mm
                }
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
            Q.Label {
                id: usersTitle
                font.pointSize: window.fontBigger
                text: back.title
                anchors.centerIn: parent
                y: 1 * mm
            }
        }
        footer: ListBusyIndicator {
            running: users.model.loading && !pauseAnimations
        }
    }
}
