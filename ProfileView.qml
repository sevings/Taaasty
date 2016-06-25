import QtQuick 2.7
import org.binque.taaasty 1.0

Rectangle {
    id: profileView
    color: window.backgroundColor
    onPopped: window.popFromStack()
    property int tlogId
    property string slug: ''
    property Tlog tlog: Tlog {
        tlogId: profileView.tlogId
        slug: profileView.slug
    }
    property Author author: tlog.author
    signal popped
    property bool poppable
    Splash {
        visible: !column.visible
    }
    Component.onCompleted: {
        if (tlog.tlogId && tlog.slug)
            tlog.author.checkStatus();
    }
    Flickable {
        id: flickable
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick
        bottomMargin: 1 * mm
        contentWidth: parent.width
        contentHeight: column.height
        Poppable {
            body: profileView
        }
        Column {
            id: column
            anchors.left: parent.left
            anchors.right: parent.right
            spacing: 1 * mm
            visible: !tlog.loading
            MyImage {
                id: bigAvatar
                anchors.topMargin: 2 * mm
                anchors.bottomMargin: 2 * mm
                anchors.left: parent.left
                anchors.right: parent.right
                height: width
                url: author.largePic
                savable: true
                popBody: profileView
            }
            ThemedText {
                id: name
                anchors.left: parent.left
                anchors.right: parent.right
                font.pointSize: window.fontBiggest
                horizontalAlignment: Text.AlignHCenter
                text: author.name
                height: text.length > 0 ? paintedHeight : 0
            }
            ThemedText {
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter
                text: author.title
                height: text.length > 0 ? paintedHeight : 0
            }
            ThemedText {
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter
                text: author.isFemale ? 'Девушка' : 'Парень'
                height: text.length > 0 ? paintedHeight : 0
                visible: !author.isFlow
            }
            ThemedText {
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter
                text: author.lastSeenAt
                height: text.length > 0 ? paintedHeight : 0
                visible: !author.isFlow
            }
            ThemedText {
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter
                text: author.isPrivacy ? 'Закрытый тлог' : 'Открытый тлог'
                height: text.length > 0 ? paintedHeight : 0
                visible: !author.isFlow
            }
            ThemedText {
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter
                text: author.daysCount
                height: text.length > 0 ? paintedHeight : 0
            }
            ThemedText {
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter
                text: {
                    switch (tlog.myRelationship) {
                    case Tlog.Friend:
                        'Вы подписаны';         break;
                    case Tlog.None:
                        'Вы не подписаны';      break;
                    case Tlog.Me:
                        'Это вы';               break;
                    case Tlog.Ignored:
                        'Тлог заблокирован';    break;
                    default:
                        '';
                    }
                }

                height: text.length > 0 ? paintedHeight : 0
                visible: tlog.tlogId === author.id && tlog.myRelationship !== Tlog.Undefined
            }
            ThemedText {
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter
                text: tlog.hisRelationship === Tlog.Friend ? 'Следит за вашим тлогом'
                                                           : 'Не следит за вашим тлогом'
                height: text.length > 0 ? paintedHeight : 0
                visible: tlog.tlogId === author.id && !author.isFlow
                         && tlog.hisRelationship !== Tlog.Undefined && tlog.hisRelationship !== Tlog.Me
            }
            Item {
                anchors.left: parent.left
                anchors.right: parent.right
                height: tlogVoteButton.height
                property int tlogMode: Trainer.typeOfTlog(tlog.tlogId)
//                onTlogModeChanged: console.log('tlog mode', tlogMode)
                ThemedButton {
                    id: tlogVoteAgainstButton
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.topMargin: 0
                    height: 6 * mm
                    width: (parent.width - 3 * mm) / 2
                    text: 'Фу…'
                    enabled: glowing || parent.tlogMode == 2 //Trainer.UndefinedMode
                    glowing: parent.tlogMode == 0 //Trainer.WaterMode
                    glowColor: window.darkRed
                    onClicked: {
                        parent.tlogMode = 0;
                        Trainer.trainTlog(tlog.tlogId, 0);
//                        window.pushTrainingProgress();
                    }
                }
                ThemedButton {
                    id: tlogVoteButton
                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.topMargin: 0
                    height: tlogVoteAgainstButton.height
                    width: tlogVoteAgainstButton.width
                    text: 'Да!'
                    enabled: glowing || parent.tlogMode == 2 //Trainer.UndefinedMode
                    glowing: parent.tlogMode == 1 //Trainer.FireMode
                    onClicked: {
                        parent.tlogMode = 1;
                        Trainer.trainTlog(tlog.tlogId, 1);
                    }
                }
            }
            ThemedButton {
                anchors.left: parent.left
                anchors.right: parent.right
                text: author.isFlow ? 'Читать' : author.publicEntriesCount
                onClicked: window.pushTlog(tlog.tlogId)
                enabled: !author.isPrivacy || tlog.myRelationship === Tlog.Friend || tlog.myRelationship === Tlog.Me
            }
            ThemedButton {
                anchors.left: parent.left
                anchors.right: parent.right
                text: tlog.followersCount
                onClicked: window.pushUsers(UsersModel.FollowersMode, author.id, tlog)
                visible: tlog.tlogId === author.id && !author.isFlow
            }
            ThemedButton {
                anchors.left: parent.left
                anchors.right: parent.right
                text: tlog.followingsCount
                onClicked: window.pushUsers(UsersModel.FollowingsMode, author.id, tlog)
                visible: tlog.tlogId === author.id && !author.isFlow
            }
        }
    }
}
