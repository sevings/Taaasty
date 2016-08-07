import QtQuick 2.7
import org.binque.taaasty 1.0

Rectangle {
    id: menu
    anchors {
        top: parent.top
        bottom: parent.bottom
        left: parent.left
    }
    width: 40 * mm
    color: window.backgroundColor
    signal modeChanged(int mode)
    signal flowsRequested
    signal tlogRequested
    signal chatsRequested
    signal searchRequested
    signal settingsRequested
    signal waterTlogsRequested
    signal fireTlogsRequested
    signal loginRequested
    signal aboutRequested
    Flickable {
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick
        topMargin: 1 * mm
        bottomMargin: 1 * mm
        contentWidth: parent.width
        contentHeight: column.height
        Column {
            id: column
            width: parent.width
            spacing: 1 * mm
            MenuItem {
                text: 'Мой тлог'
                onTriggered: { modeChanged(FeedModel.MyTlogMode) }
                visible: Tasty.isAuthorized
            }
            MenuItem {
                text: 'Подписки'
                onTriggered: { modeChanged(FeedModel.FriendsMode) }
                visible: Tasty.isAuthorized
            }
            MenuItem {
                text: 'Сообщения' + (Tasty.unreadChats > 0 ? ' <b>+' + Tasty.unreadChats + '</b>' : '')
                onTriggered: { chatsRequested() }
                highlighted: Tasty.unreadChats > 0
                visible: Tasty.isAuthorized
            }
            MenuSeparator {
                visible: Tasty.isAuthorized
            }
            MenuItem {
                text: 'Прямой эфир'
                onTriggered: { modeChanged(FeedModel.LiveMode) }
            }
            MenuItem {
                text: 'Лучшее'
                onTriggered: { modeChanged(FeedModel.BestMode) }
            }
            MenuItem {
                text: 'Анонимки'
                onTriggered: { modeChanged(FeedModel.AnonymousMode) }
            }
            MenuItem {
                text: 'Потоки'
                onTriggered: { flowsRequested(); }
            }
            MenuItem {
                text: 'Тлог'
                onTriggered: { tlogRequested() }
            }
            MenuSeparator { }
            MenuItem {
                text: 'Избранное'
                onTriggered: { modeChanged(FeedModel.FavoritesMode) }
                visible: Tasty.isAuthorized
            }
//            MenuSeparator { }
//            ThemedButton {
//                anchors.left: parent.left
//                anchors.right: parent.right
//                text: 'Неинтересные'
//                onClicked: { waterTlogsRequested() }
//            }
//            ThemedButton {
//                anchors.left: parent.left
//                anchors.right: parent.right
//                text: 'Интересные'
//                onClicked: { fireTlogsRequested() }
//            }
            MenuSeparator { }
            MenuItem {
                text: 'Настройки'
                onTriggered: { settingsRequested(); }
            }
            MenuItem {
                text: 'Войти'
                visible: !Tasty.isAuthorized
                onTriggered: { loginRequested() }
            }
        }
    }
}
