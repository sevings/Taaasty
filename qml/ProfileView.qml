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
                width: window.width
                height: width
                url: author.largePic
                savable: true
                popBody: profileView
            }
            ThemedText {
                id: name
                width: window.width
                font.pointSize: window.fontBiggest
                horizontalAlignment: Text.AlignHCenter
                text: author.name
                height: text.length > 0 ? paintedHeight : 0
            }
            ThemedText {
                width: window.width
                horizontalAlignment: Text.AlignHCenter
                text: author.title
                height: text.length > 0 ? paintedHeight : 0
            }
            ThemedText {
                width: window.width
                horizontalAlignment: Text.AlignHCenter
                text: author.isFemale ? 'Девушка' : 'Парень'
                height: text.length > 0 ? paintedHeight : 0
                visible: !author.isFlow
            }
            ThemedText {
                width: window.width
                horizontalAlignment: Text.AlignHCenter
                text: author.lastSeenAt
                height: text.length > 0 ? paintedHeight : 0
                visible: !author.isFlow
            }
            ThemedText {
                width: window.width
                horizontalAlignment: Text.AlignHCenter
                text: author.isPrivacy ? 'Закрытый тлог' : 'Открытый тлог'
                height: text.length > 0 ? paintedHeight : 0
                visible: !author.isFlow
            }
            ThemedText {
                width: window.width
                horizontalAlignment: Text.AlignHCenter
                text: author.daysCount
                height: text.length > 0 ? paintedHeight : 0
            }
            ThemedText {
                width: window.width
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
                width: window.width
                horizontalAlignment: Text.AlignHCenter
                text: tlog.hisRelationship === Tlog.Friend ? 'Следит за вашим тлогом'
                                                           : 'Не следит за вашим тлогом'
                height: text.length > 0 ? paintedHeight : 0
                visible: tlog.tlogId === author.id && !author.isFlow
                         && tlog.hisRelationship !== Tlog.Undefined && tlog.hisRelationship !== Tlog.Me
                         && Tasty.isAuthorized
            }
            Item {
                width: window.width
                height: tlogVoteButton.height
                property int tlogMode: Trainer.typeOfTlog(tlog.tlogId)
                visible: readButton.enabled
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
                          + '72.png'
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
                          + '72.png'
                    enabled: parent.tlogMode == 1 || parent.tlogMode == 2 //Trainer.UndefinedMode
                    onClicked: {
                        parent.tlogMode = 1;
                        Trainer.trainTlog(tlog.tlogId, tlog.author.name, 1);
                    }
                }
            }
            ThemedButton {
                id: readButton
                anchors.horizontalCenter: parent.horizontalCenter
                width: 40 * mm
                text: author.isFlow ? 'Читать' : author.publicEntriesCount
                onClicked: window.pushTlog(author.id)
                enabled: !author.isPrivacy || tlog.myRelationship === Tlog.Friend || tlog.myRelationship === Tlog.Me
            }
            ThemedButton {
                anchors.horizontalCenter: parent.horizontalCenter
                width: 40 * mm
                text: tlog.favoritesCount
                onClicked: window.pushTlog(author.id, 0, '', FeedModel.FavoritesMode)
                visible: tlog.tlogId === author.id && !author.isFlow
                enabled: readButton.enabled
            }
            ThemedButton {
                anchors.horizontalCenter: parent.horizontalCenter
                width: 40 * mm
                text: tlog.privateEntriesCount
                onClicked: window.pushTlog(author.id, 0, '', FeedModel.MyPrivateMode)
                visible: tlog.tlogId === author.id && !author.isFlow
                enabled: tlog.myRelationship === Tlog.Me
            }
            ThemedButton {
                anchors.horizontalCenter: parent.horizontalCenter
                width: 40 * mm
                text: tlog.followersCount
                onClicked: window.pushUsers(UsersModel.FollowersMode, author.id, tlog)
                visible: tlog.tlogId === author.id && !author.isFlow
            }
            ThemedButton {
                anchors.horizontalCenter: parent.horizontalCenter
                width: 40 * mm
                text: tlog.followingsCount
                onClicked: window.pushUsers(UsersModel.FollowingsMode, author.id, tlog)
                visible: tlog.tlogId === author.id && !author.isFlow
            }
            ThemedButton {
                anchors.horizontalCenter: parent.horizontalCenter
                width: 40 * mm
                text: tlog.ignoredCount
                onClicked: window.pushUsers(UsersModel.MyIgnoredMode, author.id, tlog)
                visible: tlog.myRelationship === Tlog.Me
            }
        }
    }
}
