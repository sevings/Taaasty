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
            MenuSeparator { }
            MenuItem {
                text: 'Анонимки'
                onTriggered: { modeChanged(FeedModel.AnonymousMode) }
            }
            MenuItem {
                text: 'Прямой эфир'
                onTriggered: { modeChanged(FeedModel.LiveMode) }
            }
            MenuItem {
                text: 'Лучшее'
                onTriggered: { modeChanged(FeedModel.BestMode) }
            }
            MenuSeparator { }
            MenuItem {
                text: 'Избранное'
                onTriggered: { modeChanged(FeedModel.FavoritesMode) }
                visible: Tasty.isAuthorized
            }
            MenuItem {
                text: 'Тлог'
                onTriggered: { tlogRequested() }
            }
            MenuSeparator { }
//            Item {
//                id: space
//                anchors.margins: 1 * mm
//                height: 2 * mm
//                width: parent.width
//            }
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
            //                ThemedButton {
            //                    anchors.left: parent.left
            //                    anchors.right: parent.right
            //                    text: 'Settings'
            //                    onClicked: { area.settingsRequested() }
            //                }
            MenuItem {
                text: 'Загружать изображения'
                highlighted: Settings.autoloadImages
                onTriggered: { Settings.autoloadImages = !highlighted }
            }
            MenuItem {
                text: 'Скрывать короткие посты'
                highlighted: Settings.hideShortPosts
                onTriggered: { Settings.hideShortPosts = !highlighted }
            }
            MenuItem {
                text: 'Включить НБК'
                highlighted: Settings.hideNegativeRated
                onTriggered: { Settings.hideNegativeRated = !highlighted }
            }
            MenuItem {
                text: 'Темная тема'
                highlighted: Settings.darkTheme
                onTriggered: { Settings.darkTheme = !highlighted }
            }
            MenuItem {
                text: 'Войти'
                visible: !Tasty.isAuthorized
                onTriggered: { loginRequested() }
            }
            MenuSeparator { }
            MenuItem {
                text: 'О клиенте'
                onTriggered: { aboutRequested() }
            }
        }
    }
}
