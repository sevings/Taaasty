import QtQuick 2.7
import org.binque.taaasty 1.0

Pane {
    id: profileView
    property int tlogId
    property string slug: ''
    property Tlog tlog: Tlog {
        tlogId: profileView.tlogId
        slug: profileView.slug
    }
    property Author author: tlog.author
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
            anchors {
                left: parent.left
                right: parent.right
            }
            spacing: 1 * mm
            visible: !tlog.loading
            MyImage {
                id: bigAvatar
                anchors {
                    topMargin: 2 * mm
                    bottomMargin: 2 * mm
                    left: parent.left
                    right: parent.right
                }
                height: width
                url: author.largePic
                savable: true
                popBody: profileView
            }
            ThemedText {
                id: name
                anchors {
                    left: parent.left
                    right: parent.right
                }
                font.pointSize: window.fontBiggest
                horizontalAlignment: Text.AlignHCenter
                text: author.name
                height: text.length > 0 ? paintedHeight : 0
            }
            ThemedText {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                horizontalAlignment: Text.AlignHCenter
                text: author.title
                height: text.length > 0 ? paintedHeight : 0
            }
            ThemedText {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                horizontalAlignment: Text.AlignHCenter
                text: author.isFemale ? 'Девушка' : 'Парень'
                height: text.length > 0 ? paintedHeight : 0
                visible: !author.isFlow
            }
            ThemedText {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                horizontalAlignment: Text.AlignHCenter
                text: author.lastSeenAt
                height: text.length > 0 ? paintedHeight : 0
                visible: !author.isFlow
            }
            ThemedText {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                horizontalAlignment: Text.AlignHCenter
                text: author.isPrivacy ? 'Закрытый тлог' : 'Открытый тлог'
                height: text.length > 0 ? paintedHeight : 0
                visible: !author.isFlow
            }
            ThemedText {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                horizontalAlignment: Text.AlignHCenter
                text: author.daysCount
                height: text.length > 0 ? paintedHeight : 0
            }
            ThemedText {
                anchors {
                    left: parent.left
                    right: parent.right
                }
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
                         && Tasty.isAuthorized
            }
            ThemedText {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                horizontalAlignment: Text.AlignHCenter
                text: tlog.hisRelationship === Tlog.Friend ? 'Следит за вашим тлогом'
                                                           : 'Не следит за вашим тлогом'
                height: text.length > 0 ? paintedHeight : 0
                visible: tlog.tlogId === author.id && !author.isFlow
                         && tlog.hisRelationship !== Tlog.Undefined && tlog.hisRelationship !== Tlog.Me
                         && Tasty.isAuthorized
            }
            Item {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                height: tlogVoteButton.height
                property int tlogMode: Trainer.typeOfTlog(tlog.tlogId)
//                onTlogModeChanged: console.log('tlog mode', tlogMode)
                enabled: readButton.enabled
                ThemedButton {
                    id: tlogVoteAgainstButton
                    anchors {
                        top: parent.top
                        left: parent.left
                        topMargin: 0
                    }
                    height: 6 * mm
                    width: (parent.width - 3 * mm) / 2
                    text: 'Фу…'
                    enabled: glowing || parent.tlogMode == 2 //Trainer.UndefinedMode
                    glowing: parent.tlogMode == 0 //Trainer.WaterMode
                    glowColor: window.darkTheme ? window.darkRed : window.redColor
                    onClicked: {
                        parent.tlogMode = 0;
                        Trainer.trainTlog(tlog.tlogId, 0);
//                        window.pushTrainingProgress();
                    }
                }
                ThemedButton {
                    id: tlogVoteButton
                    anchors {
                        top: parent.top
                        right: parent.right
                        topMargin: 0
                    }
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
                id: readButton
                anchors {
                    left: parent.left
                    right: parent.right
                }
                text: author.isFlow ? 'Читать' : author.publicEntriesCount
                onClicked: window.pushTlog(author.id)
                enabled: !author.isPrivacy || tlog.myRelationship === Tlog.Friend || tlog.myRelationship === Tlog.Me
            }
            ThemedButton {
                id: calendarButton
                anchors {
                    left: parent.left
                    right: parent.right
                }
                text: 'Обзор постов'
                onClicked: window.pushCalendar(author.id)
                enabled: readButton.enabled
            }
            ThemedButton {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                text: tlog.followersCount
                onClicked: window.pushUsers(UsersModel.FollowersMode, author.id, tlog)
                visible: tlog.tlogId === author.id && !author.isFlow
            }
            ThemedButton {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                text: tlog.followingsCount
                onClicked: window.pushUsers(UsersModel.FollowingsMode, author.id, tlog)
                visible: tlog.tlogId === author.id && !author.isFlow
            }
            ThemedButton {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                text: tlog.ignoredCount
                onClicked: window.pushUsers(UsersModel.MyIgnoredMode, author.id, tlog)
                visible: tlog.myRelationship === Tlog.Me
            }
        }
    }
}
