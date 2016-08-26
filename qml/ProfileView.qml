import QtQuick 2.7
import QtQuick.Controls.Material 2.0
import org.binque.taaasty 1.0

Pane {
    id: profileView
    innerFlick: flickable
    property int tlogId
    property string slug: ''
    property Tlog tlog: Tlog {
        tlogId: profileView.tlogId
        slug: profileView.slug
    }
    property Author author: tlog.author
    readonly property bool isProfileView: true
    Splash {
        visible: !column.visible
    }
    Component.onCompleted: {
        if (!tlog.slug.length)
        {
            tlog.reload();
            if (tlog.tlogId !== author.id)
                author.reload();
        }
        else if (tlog.tlogId && tlog.slug)
            tlog.author.checkStatus();
    }
    Connections {
        target: tlog
        onUpdated: {
            if (tlog.tlogId === author.id)
                author = tlog.author;
        }
    }
    MyFlickable {
        id: flickable
        anchors.fill: parent
        bottomMargin: 1 * mm
        contentWidth: parent.width
        contentHeight: column.height
//        interactive: profileView.x == 0
        Poppable {
            body: profileView
        }
        Column {
            id: column
            width: window.width
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
                enabled: readButton.enabled
                IconButton {
                    id: tlogVoteAgainstButton
                    anchors {
                        top: parent.top
                        left: parent.left
                        topMargin: 0
                    }
                    width: (parent.width - 3 * mm) / 2
                    icon: (parent.tlogMode == 0 //Trainer.WaterMode
                          ? '../icons/drop-solid-' : '../icons/drop-outline-')
                          + (dp < 2 ? '36' : '72') + '.png'
                    enabled: parent.tlogMode == 0 || parent.tlogMode == 2 //Trainer.UndefinedMode
                    onClicked: {
                        parent.tlogMode = 0;
                        Trainer.trainTlog(tlog.tlogId, tlog.author.name, 0);
                    }
                }
                IconButton {
                    id: tlogVoteButton
                    anchors {
                        top: parent.top
                        right: parent.right
                        topMargin: 0
                    }
                    width: tlogVoteAgainstButton.width
                    icon: (parent.tlogMode == 1 //Trainer.FireMode
                          ? '../icons/flame-solid-' : '../icons/flame-outline-')
                          + (dp < 2 ? '36' : '72') + '.png'
                    enabled: parent.tlogMode == 1 || parent.tlogMode == 2 //Trainer.UndefinedMode
                    onClicked: {
                        parent.tlogMode = 1;
                        Trainer.trainTlog(tlog.tlogId, tlog.author.name, 1);
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
